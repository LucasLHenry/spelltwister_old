#include <Arduino.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>
#include <OneButton.h>

#include "src/hardware/register_functions.h"
#include "src/hardware/pins.h"
#include "src/objects/led_ring.h"
#include "src/signals/generator.h"
#include "src/objects/module.h"
#include "src/signals/mod_algorithms.h"
#include "src/hardware/config.h"
#include "src/objects/modulator.h"
#include "src/signals/misc_algorithms.h"

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
LedRing* _LEDRING = &ring; // used for internal ISR stuff
Module A(LIN_TIME_A, MUX_A, true);
Module B(LIN_TIME_B, MUX_B, false);
Modulator modulator(A, B, ring, algo_arr);
OneButton follow_btn(FLW_BTN, false, false);

uint64_t loop_counter = 0;
uint64_t isr_counter = 0;
// this will change as more code is added to loop, not fully accurate
const uint64_t loops_per_sec = 460;
uint64_t runtime_s = 0;


void A_sync_ISR() {
    A.reset();
    A.running = true;
}

void B_sync_ISR() {
    B.reset();
    B.running = true;
}

void follow_ISR() {
    B.follow = !B.follow;
    if (B.follow) B.acc = A.acc;
}

void write_signal_indicator_leds(Adafruit_NeoPixel_ZeroDMA& leds, Module& A, Module& B, Modulator& modulator);

void setup() {
    Serial.begin(9600);
    leds.begin();
    ring.begin();

    setup_timers();

    attachInterrupt(digitalPinToInterrupt(SIG_IN_A), A_sync_ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(SIG_IN_B), B_sync_ISR, FALLING);
    follow_btn.attachClick(follow_ISR);

    update_values_from_config(ring, A, B);

    pinMode(TRIG_OUT_A, OUTPUT);
    pinMode(TRIG_OUT_B, OUTPUT);
}

void loop() {
    follow_btn.tick();
    // get new values from pots and CV
    A.read_inputs_frequent(B);
    B.read_inputs_frequent(A);

    if (loop_counter % 20 == 0) {
        A.read_inputs_infrequent();
        B.read_inputs_infrequent();
    }
    
    ring.update(A.mod_idx, B.mod_idx);
    ring.write_leds(leds);
    write_signal_indicator_leds(leds, A, B, modulator);
    leds.show();
    if (runtime_s % CONFIG_WRITE_PERIOD_S == 0) write_encoder_to_config(ring);

    loop_counter++;
    if (loop_counter > loops_per_sec) {
        loop_counter = 0;
        runtime_s++;
    }
}


// called at 48kHz, generates the signal samples
void TCC0_Handler() {
    if (TCC0->INTFLAG.bit.CNT == 1) {
        A.update();
        B.update();

        A_PRI_REG = 1023 - (A.generate() >> 6);
        B_PRI_REG = 1023 - (B.generate() >> 6);
        A_SEC_REG = 1023 - (modulator.generate_A() >> 6);
        B_SEC_REG = 1023 - (modulator.generate_B() >> 6);

        if (A.prev_eos != A.end_of_cycle) digitalWriteDirect(TRIG_OUT_A, !A.end_of_cycle);
        if (B.prev_eos != B.end_of_cycle) digitalWriteDirect(TRIG_OUT_B, !B.end_of_cycle);
        isr_counter++;

        TCC0->INTFLAG.bit.CNT = 1;
    }
}

#define RECT(x) (((x) > 0)? (x) : -(x))
void write_signal_indicator_leds(Adafruit_NeoPixel_ZeroDMA& leds, Module& A, Module& B, Modulator& modulator) {
    // if (A.mode == ENV) {
    //     leds.setPixelColor(PRI_A_LED, (A.val - HALF_Y) >> 8, 0, 0);
    //     leds.setPixelColor(SEC_A_LED, (modulator.a_val - HALF_Y) >> 8, 0, 0);
    // } else {
    //     leds.setPixelColor(PRI_A_LED, A.val >> 8, 0, 0);
    //     leds.setPixelColor(SEC_A_LED, modulator.a_val >> 8, 0, 0); 
    // }
    leds.setPixelColor(PRI_A_LED, RECT(static_cast<int32_t>(A.val - HALF_Y)) >> 8, 0, 0);
    leds.setPixelColor(SEC_A_LED, RECT(static_cast<int32_t>(modulator.a_val - HALF_Y)) >> 8, 0, 0);

    leds.setPixelColor(PRI_B_LED, 0, 0, RECT(static_cast<int32_t>(B.val - HALF_Y)) >> 8);
    leds.setPixelColor(SEC_B_LED, 0, 0, RECT(static_cast<int32_t>(modulator.b_val - HALF_Y)) >> 8);

    // if (B.mode == ENV) {
    //     leds.setPixelColor(PRI_B_LED, 0, 0, RECT(static_cast<int32-t>(B.val - HALF_Y)) >> 8);
    //     leds.setPixelColor(SEC_B_LED, 0, 0, RECT(static_cast<int32-t>(modulator.b_val - HALF_Y)) >> 8);
    // } else {
    //     leds.setPixelColor(PRI_B_LED, 0, 0, B.val >> 8);
    //     leds.setPixelColor(SEC_B_LED, 0, 0, modulator.b_val >> 8);
    // }

    leds.setPixelColor(TRIG_A_LED, (A.eos_led)? RED : BLACK);
    leds.setPixelColor(TRIG_B_LED, (B.eos_led)? BLUE : BLACK);

    leds.setPixelColor(FLW_LED, (B.follow)? PURPLE : BLACK);
}