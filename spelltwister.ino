#include <Arduino.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>

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

uint64_t loop_counter = 0;
uint64_t isr_counter = 0;
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

void write_signal_indicator_leds(Adafruit_NeoPixel_ZeroDMA& leds, Module& A, Module& B, Modulator& modulator);

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
    // get new values from pots and CV
    A.read_inputs_frequent();
    B.read_inputs_frequent();

    if (loop_counter % 20 == 0) {
        A.read_inputs_infrequent();
        B.read_inputs_infrequent();
    }
    
    ring.update();
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

        // not is in there because of an output inverter
        digitalWrite(TRIG_OUT_A, !A.end_of_cycle);
        digitalWrite(TRIG_OUT_B, !B.end_of_cycle);
        isr_counter++;
        TCC0->INTFLAG.bit.CNT = 1;
    }
}

void write_signal_indicator_leds(Adafruit_NeoPixel_ZeroDMA& leds, Module& A, Module& B, Modulator& modulator) {
    #define MAX_PERCEIVABLE_FREQUENCY_Hz 40
    constexpr uint32_t max_phasor = MAX_PERCEIVABLE_FREQUENCY_Hz * HZPHASOR;

    // if (A.pha > max_phasor) {
    //     leds.setPixelColor(PRI_A_LED, RED);
    //     leds.setPixelColor(SEC_A_LED, RED);
    // } else {
        leds.setPixelColor(PRI_A_LED, better_dimmer(A.val >> 11, RED_HSL));
        leds.setPixelColor(SEC_A_LED, better_dimmer(modulator.a_val >> 11, RED_HSL));
    // }

    // if (B.pha > max_phasor) {
    //     leds.setPixelColor(PRI_B_LED, BLUE);
    //     leds.setPixelColor(SEC_B_LED, BLUE);
    // } else {
        leds.setPixelColor(PRI_B_LED, better_dimmer(B.val >> 11, BLUE_HSL));
        leds.setPixelColor(SEC_B_LED, better_dimmer(modulator.b_val >> 11, BLUE_HSL));
    // }

    if (A.eos_led) {
        leds.setPixelColor(TRIG_A_LED, RED);
    } else {
        leds.setPixelColor(TRIG_A_LED, BLACK);
    }

    if (B.eos_led) {
        leds.setPixelColor(TRIG_B_LED, BLUE);
    } else {
        leds.setPixelColor(TRIG_B_LED, BLACK);
    }
}