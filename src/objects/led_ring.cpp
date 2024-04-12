#include "led_ring.h"

void LedRing::write_leds(Adafruit_NeoPixel_ZeroDMA &leds) {
    if (a_idx == b_idx) {
        for (int i = 0; i < NUM_RING_LEDS; i++) {
            if (i == a_idx) leds.setPixelColor(i,  PURPLE);
            else leds.setPixelColor(i, BLACK);
        }
    } else {
        for (int i = 0; i < NUM_RING_LEDS; i++) {
            if (i == a_idx) leds.setPixelColor(i, RED);
            else if (i == b_idx) leds.setPixelColor(i, BLUE);
            else leds.setPixelColor(i, BLACK);
        }
    }
}

void LedRing::update() {
    if (enc_change == 0) return;
    if (a_is_active) {
        a_idx += enc_change;
        a_idx %= NUM_RING_LEDS;
        if (a_idx < 0) a_idx += 16;
    } else {
        b_idx += enc_change;
        b_idx %= NUM_RING_LEDS;
        if (b_idx < 0) b_idx += 16;
    }
    enc_change = 0;
}

void encoder_handler(EncoderButton &btn) {
    _LEDRING->enc_change = btn.increment();
    Serial.println(_LEDRING->enc_change);
}

void button_handler(EncoderButton &btn) {
    _LEDRING->a_is_active = !_LEDRING->a_is_active;
}


void setup_handlers(EncoderButton &btn, LedRing &ring) {
    btn.setEncoderHandler(encoder_handler);
    btn.setClickHandler(button_handler);
}