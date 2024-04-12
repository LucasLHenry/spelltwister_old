#include <Arduino.h>
#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>
#include <OneButton.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>

#include "../hardware/pins.h"
#include "../colours.h"

#ifndef LED_RING_H
#define LED_RING_H

#define ENC_DIV 2

class LedRing {
    int64_t a_pos_raw, b_pos_raw;
    int64_t enc_pos, new_enc_pos;
    int64_t enc_change;
    int16_t a_idx, b_idx;
    Encoder enc;
    OneButton btn;
    public:
        bool a_is_active;
        void write_leds(Adafruit_NeoPixel_ZeroDMA &leds);
        void begin();
        void update();
        LedRing(int enc_pin_1, int enc_ping_2, int btn_pin);

};

void button_handler();

extern LedRing* _LEDRING;

#endif