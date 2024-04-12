#include <Arduino.h>
#include <EncoderButton.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>

#include "../hardware/pins.h"
#include "../colours.h"

#ifndef LED_RING_H
#define LED_RING_H

#define ENC_DIV 2

class LedRing {
    int64_t a_pos_raw, b_pos_raw;
    public:
        int16_t a_idx, b_idx;
        bool a_is_active;
        int16_t enc_change;
        void write_leds(Adafruit_NeoPixel_ZeroDMA &leds);
        void update();
        LedRing() = default;
};

void setup_handlers(EncoderButton &btn, LedRing &ring);
void encoder_handler(EncoderButton &btn);
void button_handler(EncoderButton &btn);

extern LedRing* _LEDRING;

#endif