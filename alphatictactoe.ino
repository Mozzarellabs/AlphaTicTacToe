#include <stdlib.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define POWER 32
#define DELAY 50

#define COLOR_NONE 0,POWER,POWER
#define COLOR_PLAYER_1 0,POWER,0
#define COLOR_PLAYER_2 POWER,0,0

#define BUTTON_START 2
#define BUTTON_END 11
#define PIN 11

// Parameter 1 = number of pixels in _strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel _strip = Adafruit_NeoPixel(12 * 9, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

typedef struct
{
    uint8_t played;
    uint8_t won;
    uint8_t buttonsState[9]; // 1 if pressed during the current loop, 2 for released
    uint8_t buttonsPrev[9]; // Value at the previous loop
    uint32_t color[9]; // Desired color
} UIState;
UIState* _ui;

typedef struct
{
    void (*func)(void*);
    void* data;
    void* next;
} Action;
Action* _actions;
Action* _lastAction;

uint8_t _buttonStates[9];
void (*IA_func)(void*);

void UIState_init(void* u)
{
    UIState* ui = (UIState*)u;
    for (int i = 0; i < 9; ++i)
    {
        ui->played = 0;
        ui->won = 0;
        ui->buttonsState[i] = 0;
        ui->buttonsPrev[i] = HIGH;
        ui->color[i] = 0;
    }
}

void UIState_process_inputs(void* u)
{
    UIState* ui = (UIState*)u;
    uint8_t buttons[9];
    for (uint8_t i = 0; i < 9; ++i)
        buttons[i] = 0;

    for (uint16_t i = BUTTON_START; i < BUTTON_END; ++i)
    {
        int buttonState = digitalRead(i);
        uint16_t realButtonId = i - BUTTON_START;
        ui->buttonsPrev[realButtonId] = buttonState;
        buttons[realButtonId] = buttonState == LOW ? 1 : 0;
    }

    // Check the number of buttons pressed
    uint8_t pressed = 0;
    uint8_t lastId = 0;
    for (uint8_t i = 0; i < 9; ++i)
        if (buttons[i] == 1)
        {
            pressed++;
            lastId = i;
        }

    if (pressed == 1)
    {
        if (ui->buttonsState[lastId] == 0)
        {
            ui->played = 1;
            ui->buttonsState[lastId] = 1;
        }
    }
    else if (pressed == 2)
    {
        for (uint8_t i = 0; i < 9; ++i)
            ui->buttonsState[i] = 0;
    }
}

void UIState_update_lights(void* u)
{
    UIState* ui = (UIState*)u;

    for (uint16_t i = 0; i < 9; ++i)
    {
        if (ui->buttonsState[i] == 1)
            ui->color[i] = _strip.Color(COLOR_PLAYER_1);
        if (ui->buttonsState[i] == 2)
            ui->color[i] = _strip.Color(COLOR_PLAYER_2);
        else if (ui->buttonsState[i] == 0)
            ui->color[i] = _strip.Color(COLOR_NONE);
        set_button_color(i, ui->color[i]);
    }
    _strip.show();
}

void UIState_set_all_lights(void* u, uint32_t color)
{
    UIState* ui = (UIState*)u;

    for (uint8_t i = 0; i < 9; ++i)
    {
        ui->color[i] = color;
        set_button_color(i, ui->color[i]);
        delay(100);
        _strip.show();
    }
}

int UIState_check_win(void* u, uint8_t player)
{
    UIState* ui = (UIState*)u;

    if (player != 1 && player != 2)
        return 0;

    uint8_t state[9];
    for (uint8_t i = 0; i < 9; ++i)
        if (ui->buttonsState[i] == player)
            state[i] = 1;
        else
            state[i] = 0;

    // Check for horizontal and vertical lines
    uint8_t inLine = 0;
    for (uint8_t i = 0; i < 3; ++i)
    {
        inLine = 0;
        for (uint8_t j = 0; j < 3; ++j)
            if (state[i*3 + j] == 1)
                inLine++;

        if (inLine == 3)
            ui->won = 1;

        inLine = 0;
        for (uint8_t j = 0; j < 3; ++j)
            if (state[j*3 + i] == 1)
                inLine++;

        if (inLine == 3)
            ui->won = 1;
    }

    // Check for diagonals
    inLine = 0;
    for (uint8_t i = 0; i < 3; ++i)
        if (state[i*3 + i] == 1)
            inLine++;

    if (inLine == 3)
        ui->won = 1;

    inLine = 0;
    for (uint8_t i = 0; i < 3; ++i)
        if (state[i*3 + (2-i)] == 1)
            inLine++;

    if (inLine == 3)
        ui->won = 1;

    if (ui->won == 1)
        return 1;
    else
        return 0;
}

void UIState_play_IA_random(void* u)
{
    UIState* ui = (UIState*)u;

    delay(1000);
    uint8_t played = 0;
    while (!played)
    {
        uint8_t id = rand() / ((RAND_MAX - 1) / 9);
        if (ui->buttonsState[id] == 0)
        {
            played = 1;
            ui->buttonsState[id] = 2;
        }
    }
}

void UIState_reset_play(void* u)
{
    UIState* ui = (UIState*)u;

    ui->played = 0;
}

void set_button_color(uint16_t id, uint32_t color)
{
    for (uint8_t i = id * 12; i < (id + 1) * 12; ++i)
        _strip.setPixelColor(i, color);
}

void setup() {
    // Debug stuff
    //Serial.begin(9600);

    // Buttons
    for (uint8_t i = BUTTON_START; i < BUTTON_END; ++i)
        pinMode(i, INPUT_PULLUP);

    // UI
    _ui = new UIState;
    UIState_init(_ui);

    // IA
    IA_func = &UIState_play_IA_random;

    // Neopixel init
    _strip.begin();
    _strip.show(); // Initialize all pixels to 'off'
}

void loop() {
    UIState_process_inputs(_ui);

    if (_ui->won == 0)
    {
        UIState_update_lights(_ui);

        // If the played did not do anything, loop
        if (!_ui->played)
        {
            delay(DELAY);
            return;
        }
        UIState_reset_play(_ui);

        // Check win
        if (UIState_check_win(_ui, 1))
        {
            UIState_set_all_lights(_ui, _strip.Color(COLOR_PLAYER_1));
            return;
        }

        IA_func(_ui);

        // Check win
        if (UIState_check_win(_ui, 2))
        {
            UIState_set_all_lights(_ui, _strip.Color(COLOR_PLAYER_2));
            return;
        }
        UIState_reset_play(_ui);
    }

    delay(DELAY);
}
