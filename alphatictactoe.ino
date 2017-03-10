#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include "./state.h"
#include "./ia.h"

#define POWER 32
#define DELAY 50

#define BLACK 0, 0, 0
#define COLOR_NONE 0,POWER/2,POWER/2
#define COLOR_PLAYER_1 0,POWER,0
#define COLOR_PLAYER_2 POWER,0,0
#define DRAW POWER/4,POWER/4,POWER/4
#define SWITCH POWER/2, POWER/4, 0

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

UIState* _ui;

//
// State stuff
//
void UIState_init(void* u)
{
    UIState* ui = (UIState*)u;

    ui->iaLevel = 0;
    ui->played = 0;
    ui->won = 0;
    ui->coupPlayed = 0;
    for (int i = 0; i < 9; ++i)
    {
        ui->buttonsState[i] = 0;
        ui->buttonsPrev[i] = HIGH;
        ui->color[i] = 0;
    }
}

void UIState_reset(void* u)
{
    UIState* ui = (UIState*)u;

    ui->won = 0;
    ui->coupPlayed = 0;
    for (uint8_t i = 0; i < 9; ++i)
        ui->buttonsState[i] = 0;
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

//
// Light stuff
//
void set_button_color(uint16_t id, uint32_t color)
{
    for (uint8_t i = id * 12; i < (id + 1) * 12; ++i)
        _strip.setPixelColor(i, color);
}

void UIState_set_all_lights(void* u, uint32_t color, int time = 100, int stayDelay = 3000)
{
    UIState* ui = (UIState*)u;

    for (uint8_t i = 0; i < 9; ++i)
    {
        ui->color[i] = color;
        set_button_color(i, ui->color[i]);
        delay(time);
        _strip.show();
    }
    delay(stayDelay);
}

void UIState_set_rows(void* u, uint32_t color, uint8_t row_count, int stayDelay = 500)
{
    UIState* ui = (UIState*)u;

    UIState_set_all_lights(u, _strip.Color(BLACK), 0, 0);
    for (uint8_t i = 0; i < row_count; ++i)
        for (uint8_t y = 0; y < 3; ++y)
        {
            ui->color[y*3 + i] = color;
            set_button_color(y*3 + i, color);
        }
    _strip.show();
    delay(stayDelay);
}

//
// I/O stuff\
//
void UIState_process_inputs(void* u, uint8_t playerId)
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

    int potard = analogRead(A4);
    // TODO: change default color based on IA
    if (potard < 255)
    {
        if (ui->iaLevel != 0)
        {
            ui->iaLevel = 0;
            ui->IA_func = UIState_play_IA_noplay;
            UIState_set_rows(u, _strip.Color(SWITCH), 0);
            UIState_reset(ui);
            UIState_set_all_lights(ui, _strip.Color(COLOR_NONE), 10, 0);
        }
    }
    else if (potard < 512)
    {
        if (ui->iaLevel != 1)
        {
            ui->iaLevel = 1;
            ui->IA_func = UIState_play_IA_random;
            UIState_set_rows(u, _strip.Color(SWITCH), 1);
            UIState_reset(ui);
            UIState_set_all_lights(ui, _strip.Color(COLOR_NONE), 10, 0);
        }
    }
    else if (potard < 768)
    {
        if (ui->iaLevel != 2)
        {
            ui->iaLevel = 2;
            ui->IA_func = UIState_play_IA_semi_random;
            UIState_set_rows(u, _strip.Color(SWITCH), 2);
            UIState_reset(ui);
            UIState_set_all_lights(ui, _strip.Color(COLOR_NONE), 10, 0);
        }
    }
    else
    {
        if (ui->iaLevel != 3)
        {
            ui->iaLevel = 3;
            ui->IA_func = UIState_play_IA_semi_random;
            UIState_set_rows(u, _strip.Color(SWITCH), 3);
            UIState_reset(ui);
            UIState_set_all_lights(ui, _strip.Color(COLOR_NONE), 10, 0);
        }
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
            ui->buttonsState[lastId] = playerId;
            ui->coupPlayed++;
        }
    }
}

//
// Gameplay stuff
//
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

    // If nobody won
    if (ui->won == 0 && ui->coupPlayed == 9)
        return -1;
    else if (ui->won == 1)
        return 1;
    else
        return 0;
}

//
// Runtime stuff
//
void setup() {
    // Debug stuff
    Serial.begin(9600);

    // Buttons
    for (uint8_t i = BUTTON_START; i < BUTTON_END; ++i)
        pinMode(i, INPUT_PULLUP);

    // Selector
    analogReference(DEFAULT);
    pinMode(A4, INPUT);

    // UI
    _ui = new UIState;
    UIState_init(_ui);

    // IA
    pinMode(0, INPUT);
    randomSeed(analogRead(0));
    _ui->IA_func = &UIState_play_IA_semi_random;

    // Neopixel init
    _strip.begin();
    _strip.show(); // Initialize all pixels to 'off'
}

void loop() {
    UIState_update_lights(_ui);

    // First player
    // If the played did not do anything, loop
    while (!_ui->played)
    {
        delay(DELAY);
        UIState_process_inputs(_ui, 1);
    }
    _ui->played = 0;
    UIState_update_lights(_ui);

    // Check win
    int winState = UIState_check_win(_ui, 1);
    if (winState == 1)
    {
        UIState_set_all_lights(_ui, _strip.Color(COLOR_PLAYER_1));
        UIState_reset(_ui);
        return;
    }
    else if (winState == -1)
    {
        UIState_set_all_lights(_ui, _strip.Color(DRAW));
        UIState_reset(_ui);
        return;
    }

    // Second player
    _ui->IA_func(_ui);
    // If the player did not do anything, loop
    while (!_ui->played)
    {
        delay(DELAY);
        UIState_process_inputs(_ui, 2);
    }
    _ui->played = 0;
    UIState_update_lights(_ui);

    // Check win
    winState = UIState_check_win(_ui, 2);
    if (winState == 1)
    {
        UIState_set_all_lights(_ui, _strip.Color(COLOR_PLAYER_2));
        UIState_reset(_ui);
        return;
    }
    else if (winState == -1)
    {
        UIState_set_all_lights(_ui, _strip.Color(DRAW));
        UIState_reset(_ui);
        return;
    }

    delay(DELAY);
}
