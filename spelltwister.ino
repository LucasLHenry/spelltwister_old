#include <Arduino.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>

#include "src/hardware/register_functions.h"
#include "src/hardware/pins.h"
#include "src/objects/led_ring.h"
#include "src/signals/generator.h"
#include "src/objects/module.h"
#include "src/signals/mod_algorithms.h"
#include "src/hardware/config.h"

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

uint64_t global_count = 0;
// this will change as more code is added to loop, not fully accurate
const uint64_t loops_per_sec = 460;
uint64_t runtime_s = 0;


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
    attachInterrupt(digitalPinToInterrupt(SIG_IN_A), A_sync_ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(SIG_IN_B), B_sync_ISR, FALLING);
    update_values_from_config(ring, A, B);
}

void loop() {
    // digitalWrite(TRIG_OUT_A, HIGH);   // to test loop speed
    A.read_inputs();
    // digitalWrite(TRIG_OUT_A, LOW);
    B.read_inputs();
    ring.update();
    ring.write_leds(leds);
    leds.show();
    if (runtime_s % CONFIG_WRITE_PERIOD_S == 0) write_encoder_to_config(ring);

    global_count++;
    if (global_count > loops_per_sec) {
        global_count = 0;
        runtime_s++;
    }
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