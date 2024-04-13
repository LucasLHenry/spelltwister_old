#include <Arduino.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>

#include "src/hardware/register_functions.h"
#include "src/hardware/pins.h"
#include "src/objects/led_ring.h"
#include "src/misc/generator.h"
#include "src/objects/module.h"

// GLOBAL VARIABLES
Adafruit_NeoPixel_ZeroDMA leds(NUM_LEDS, LED_DATA, NEO_GRB);
LedRing ring(ALGO_ENC_1, ALGO_ENC_2, ALGO_BTN);
Module A(LIN_TIME_A, MUX_A, true);
Module B(LIN_TIME_B, MUX_B, false);
LedRing* _LEDRING = &ring; // used for internal ISR stuff

#define HZPHASOR 91183 //phasor value for 1 hz.

void A_sync_ISR() {
    A.acc = 0;
    A.running = true;
}

void B_sync_ISR() {
    B.acc = 0;
    B.running = true;
}

void setup() {
    leds.begin();
    ring.begin();
    setup_timers();
    B.pha = 100*HZPHASOR;
    A.pha = 75*HZPHASOR;
    attachInterrupt(digitalPinToInterrupt(SIG_IN_A), A_sync_ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(SIG_IN_B), B_sync_ISR, FALLING);
}

void loop() {
    A.read_inputs();
    B.read_inputs();
    ring.update();
    ring.write_leds(leds);
    leds.show();
}

void TCC0_Handler() {
    if (TCC0->INTFLAG.bit.CNT == 1) {
        A.update();
        B.update();
        B_PRI_REG = 1023 - (B.generate() >> 6);
        A_PRI_REG = 1023 - (A.generate() >> 6);
        TCC0->INTFLAG.bit.CNT = 1;
    }
}