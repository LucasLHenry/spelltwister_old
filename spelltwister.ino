#include <Arduino.h>
// #include <Mux.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>
#include <EncoderButton.h>

// #include "src/hardware/register_functions.h"
#include "src/hardware/pins.h"
#include "src/objects/led_ring.h"
// #include "src/colours.h"
// #include "src/objects/encoder_wrapper.h"

// GLOBAL VARIABLES
Adafruit_NeoPixel_ZeroDMA leds(NUM_LEDS, LED_DATA, NEO_GRB);
LedRing ring(ALGO_ENC_1, ALGO_ENC_2, ALGO_BTN); // default constructor
LedRing* _LEDRING = &ring;
// Encoder enc(ALGO_ENC_1, ALGO_ENC_2);
// long int enc_a_pos, enc_b_pos;
// int active_a, active_b;
// long int enc_pos, new_enc_pos, enc_change;
// #define ENC_DIV 2

// OneButton enc_btn(ALGO_BTN, true, true);
// bool enc_a_is_active = true;

// EncoderWrapper enc(ALGO_ENC_1, ALGO_ENC_2, ALGO_BTN);

// FORWARD REFERENCES
// void enc_btn_handler();
// static void update_encoder();
// static void show_leds();

void setup() {
    leds.begin();
    ring.begin();
    // setup_timers();
    Serial.begin(9600);
}

void loop() {
    ring.update();
    ring.write_leds(leds);
    leds.show();
}

// static void update_encoder() {
//     new_enc_pos = enc.read();
//     enc_change = new_enc_pos - enc_pos;
//     enc_pos = new_enc_pos;
//     if (enc_a_is_active) {
//         enc_a_pos += enc_change;
//         active_a = (enc_a_pos >> ENC_DIV) % NUM_RING_LEDS;
//         if (active_a < 0) 
//         active_a += 16;
//     } else {
//         enc_b_pos += enc_change;
//         active_b = (enc_b_pos >> ENC_DIV) % NUM_RING_LEDS;
//         if (active_b < 0) 
//         active_b += 16;
//     }
// }

// static void show_leds() {
//     // show ring LEDs
//     if (enc.a_idx == enc.b_idx) {
//         for (int i = 0; i < NUM_RING_LEDS; i++) {
//             if (i == enc.a_idx) leds.setPixelColor(i,  PURPLE);
//             else leds.setPixelColor(i, BLACK);
//         }
//     } else {
//         for (int i = 0; i < NUM_RING_LEDS; i++) {
//             if (i == enc.a_idx) leds.setPixelColor(i, RED);
//             else if (i == enc.b_idx) leds.setPixelColor(i, BLUE);
//             else leds.setPixelColor(i, BLACK);
//         }
//     }
//     // show signal indicator LEDs
//     leds.show();
// }