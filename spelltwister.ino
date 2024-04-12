#include <Arduino.h>
// #include <Mux.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>
#include <EncoderButton.h>

#include "src/hardware/register_functions.h"
#include "src/hardware/pins.h"
#include "src/objects/led_ring.h"

// GLOBAL VARIABLES
Adafruit_NeoPixel_ZeroDMA leds(NUM_LEDS, LED_DATA, NEO_GRB);
LedRing ring(ALGO_ENC_1, ALGO_ENC_2, ALGO_BTN);
LedRing* _LEDRING = &ring;

#define HZPHASOR 91183 //phasor value for 1 hz.
uint32_t acc, phasor;

void setup() {
    leds.begin();
    ring.begin();
    setup_timers();
    phasor = 100*HZPHASOR;
    acc = 0;
}

void loop() {
    ring.update();
    ring.write_leds(leds);
    leds.show();
}

void TCC0_Handler() {
    if (TCC0->INTFLAG.bit.CNT == 1) {
        acc += phasor;
        REG_TCC0_CCB2 = 1023 - (acc >> 22);
        TCC0->INTFLAG.bit.CNT = 1;
    }
}