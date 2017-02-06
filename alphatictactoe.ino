#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define BUTTON_START 2
#define BUTTON_END 10
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
    uint8_t buttonsState[9]; // 1 if pressed during the current loop, 2 for released
    uint8_t buttonsPrev[9]; // Value at the previous loop
    uint32_t color[9]; // Desired color
} UIState;
UIState* _ui;

uint8_t _buttonStates[9];

void UIState_init(void* u)
{
    UIState* ui = (UIState*)u;
    for (int i = 0; i < 9; ++i)
    {
        ui->buttonsState[i] = 0;
        ui->buttonsPrev[i] = HIGH;
        ui->color[i] = 0;
    }
}

void UIState_process_inputs(void* u)
{
    UIState* ui = (UIState*)u;

    for (uint16_t i = BUTTON_START; i < BUTTON_END; ++i)
    {
        int buttonState = digitalRead(i);
        uint16_t realButtonId = i - BUTTON_START;
        if (buttonState != ui->buttonsPrev[realButtonId] && buttonState == LOW)
            ui->buttonsState[realButtonId] = !ui->buttonsState[realButtonId];
        ui->buttonsPrev[realButtonId] = buttonState;
    }
}

void UIState_update_lights(void* u)
{
    UIState* ui = (UIState*)u;

    for (uint16_t i = 0; i < 9; ++i)
    {
        if (ui->buttonsState[i] == 1)
            ui->color[i] = _strip.Color(0, 255, 0);
        else if (ui->buttonsState[i] == 0)
            ui->color[i] = _strip.Color(255, 0, 0);
        set_button_color(i, ui->color[i]);
    }
    _strip.show();
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

    // Neopixel init
    _strip.begin();
    _strip.show(); // Initialize all pixels to 'off'
}

void loop() {
    UIState_process_inputs(_ui);
    UIState_update_lights(_ui);

    delay(33);
}
