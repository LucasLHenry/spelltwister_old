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
#include "src/settings.h"

// ALGORITHMS ON RING
// switch these out to change the algorithms on each position
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
Adafruit_NeoPixel_ZeroDMA leds(NUM_LEDS, LED_DATA, NEO_GRB);  // led strip controller
LedRing ring(ALGO_ENC_1, ALGO_ENC_2, ALGO_BTN);  // algorithm ring controller
LedRing* _LEDRING = &ring; // used for internal ISR stuff

Module A(LIN_TIME_A, MUX_A, true);  // left side controller
Module B(LIN_TIME_B, MUX_B, false); // right side controller
Modulator modulator(A, B, ring, algo_arr);  // handles modulation algorithms
OneButton follow_btn(FLW_BTN, false, false);  // button in center

uint64_t loop_counter = 0;  // increments per run of loop() function
uint64_t isr_counter = 0;   // increments per run of main audio out ISR (TCC0_Handler)

// this will change as more code is added to loop, not fully accurate
const uint64_t loops_per_sec = 460;
uint64_t runtime_s = 0;  // amount of seconds that the machine has been running for (can overflow)


// called when a falling pulse is detected on the A sync input
void A_sync_ISR() {
    A.reset();
    A.running = true;
}

// called when a falling pulse is detected on the B sync input
void B_sync_ISR() {
    B.reset();
    B.running = true;
}

// called when the follow button is pressed
void follow_ISR() {
    B.follow = !B.follow;
    if (B.follow) B.acc = A.acc;
}

// forward references
void write_signal_indicator_leds(Adafruit_NeoPixel_ZeroDMA& leds, Module& A, Module& B, Modulator& modulator);

void setup() {
    Serial.begin(9600);
    leds.begin();
    ring.begin();

    setup_timers();  // sets up TCC0_Handler call frequency and connects waveform outputs

    attachInterrupt(digitalPinToInterrupt(SIG_IN_A), A_sync_ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(SIG_IN_B), B_sync_ISR, FALLING);
    follow_btn.attachClick(follow_ISR);

    // reads config from memory (calibration data, encoder positions)
    update_values_from_config(ring, A, B);

    pinMode(TRIG_OUT_A, OUTPUT);
    pinMode(TRIG_OUT_B, OUTPUT);
}

void loop() {
    follow_btn.tick();
    
    // update valeus that change frequently
    A.read_inputs_frequent(B);
    B.read_inputs_frequent(A);


    // update values that change infrequently
    if (loop_counter % 20 == 0) {
        A.read_inputs_infrequent();
        B.read_inputs_infrequent();
    }
    
    ring.update(A.mod_idx_change, B.mod_idx_change);  // update the LED ring (takes cv values from A and B)
    
    // write values to led array, then send that data to the LEDs themselves
    ring.write_leds(leds);
    write_signal_indicator_leds(leds, A, B, modulator);
    leds.show();

    // every so often, save current encoder position
    if (runtime_s % CONFIG_WRITE_PERIOD_S == 0) write_encoder_to_config(ring);


    // update counters
    loop_counter++;
    if (loop_counter > loops_per_sec) {
        loop_counter = 0;
        runtime_s++;
    }
}


// called at 44.1 kHz, generates the signal samples
void TCC0_Handler() {
    if (TCC0->INTFLAG.bit.CNT == 1) {
        // update the A and B internal values
        A.update();
        B.update();

        // generate the output values and write them to the output registers
        A_PRI_REG = 1023 - (A.generate() >> 6);
        B_PRI_REG = 1023 - (B.generate() >> 6);
        A_SEC_REG = 1023 - (modulator.generate_A() >> 6);
        B_SEC_REG = 1023 - (modulator.generate_B() >> 6);

        // handle trig outs
        if (A.prev_eos != A.end_of_cycle) digitalWriteDirect(TRIG_OUT_A, !A.end_of_cycle);
        if (B.prev_eos != B.end_of_cycle) digitalWriteDirect(TRIG_OUT_B, !B.end_of_cycle);

        isr_counter++;

        // have to set this to keep the ISR working properly
        TCC0->INTFLAG.bit.CNT = 1;
    }
}

void write_signal_indicator_leds(Adafruit_NeoPixel_ZeroDMA& leds, Module& A, Module& B, Modulator& modulator) {
    // A is red (value, 0, 0)
    if (A.mode == ENV) {
        leds.setPixelColor(PRI_A_LED, (A.val - HALF_Y) >> 7, 0, 0);
        leds.setPixelColor(SEC_A_LED, (modulator.a_val - HALF_Y) >> 7, 0, 0);
    } else {
        leds.setPixelColor(PRI_A_LED, A.val >> 8, 0, 0);
        leds.setPixelColor(SEC_A_LED, modulator.a_val >> 8, 0, 0); 
    }

    // B is blue (0, 0, value)
    if (B.mode == ENV) {
        leds.setPixelColor(PRI_B_LED, 0, 0, (B.val - HALF_Y) >> 7);
        leds.setPixelColor(SEC_B_LED, 0, 0, (modulator.b_val - HALF_Y) >> 7);
    } else {
        leds.setPixelColor(PRI_B_LED, 0, 0, B.val >> 8);
        leds.setPixelColor(SEC_B_LED, 0, 0, modulator.b_val >> 8);
    }

    // write trig LEDs
    leds.setPixelColor(TRIG_A_LED, (A.eos_led)? RED : BLACK);
    leds.setPixelColor(TRIG_B_LED, (B.eos_led)? BLUE : BLACK);

    // write follow LED
    leds.setPixelColor(FLW_LED, (B.follow)? PURPLE : BLACK);
}