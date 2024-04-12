#include <Arduino.h>
// #include <Mux.h>
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
uint32_t acc, phasor;

uint16_t upslope = calc_upslope(128);
uint16_t downslope = calc_downslope(128);

void setup() {
    leds.begin();
    ring.begin();
    setup_timers();
    B.pha = 100*HZPHASOR;
}
uint16_t val;
void loop() {
    A.read_inputs();
    B.read_inputs();
    ring.update();
    ring.write_leds(leds);
    leds.show();
}

void TCC0_Handler() {
    if (TCC0->INTFLAG.bit.CNT == 1) {
        // A.update();
        B.update();
        REG_TCC0_CCB2 = 1023 - (B.generate() >> 6);
        TCC0->INTFLAG.bit.CNT = 1;
    }
}