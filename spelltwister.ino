#include <Arduino.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>

#include "src/hardware/register_functions.h"
#include "src/hardware/pins.h"
#include "src/objects/led_ring.h"
#include "src/signals/generator.h"
#include "src/objects/module.h"
#include "src/signals/mod_algorithms.h"

// ALGORITHMS ON RING
algo_f_ptr algo_arr[16] = {
    rectify,
    half_freq,
    double_freq,
    invert,
    exculsive_or,
    difference,
    sum,
    frequency_mod,
    amplitude_mod,
    gate,
    shape_mod,
    ratio_mod,
    wavefold,
    sample_rate_reduce,
    noisify,
    bitcrush
};

// GLOBAL VARIABLES
Adafruit_NeoPixel_ZeroDMA leds(NUM_LEDS, LED_DATA, NEO_GRB);
LedRing ring(ALGO_ENC_1, ALGO_ENC_2, ALGO_BTN);
Module A(LIN_TIME_A, MUX_A, true);
Module B(LIN_TIME_B, MUX_B, false);
LedRing* _LEDRING = &ring; // used for internal ISR stuff

void A_sync_ISR() {
    A.acc = 0;
    A.running = true;
}

void B_sync_ISR() {
    B.acc = 0;
    B.running = true;
}

void setup() {
    Serial.begin(9600);
    leds.begin();
    ring.begin();
    setup_timers();
    // B.pha = 100*HZPHASOR;
    // A.pha = 75*HZPHASOR;
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
        A_PRI_REG = 1023 - (A.generate() >> 6);
        B_PRI_REG = 1023 - (B.generate() >> 6);
        A_SEC_REG = 1023 - (algo_arr[ring.a_idx](A, B) >> 6);
        B_SEC_REG = 1023 - (algo_arr[ring.b_idx](B, A) >> 6);
        TCC0->INTFLAG.bit.CNT = 1;
    }
}