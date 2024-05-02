#include "module.h"

const int8_t mult_arr[8] = {-4, -2, 2, 3, 4, 5, 6, 8};

uint32_t Module::get_phasor(Module& other) {
    if (!is_A && follow) return other.pha;

    #define MIN_LFO_ENV_CYCLE_TIME_S 0.025
    #define MAX_LFO_ENV_CYCLE_TIME_S 10.0
    constexpr uint32_t min_lfo_env_phasor = static_cast<uint32_t>(HZPHASOR / MAX_LFO_ENV_CYCLE_TIME_S);
    constexpr uint32_t max_lfo_env_phasor = static_cast<uint32_t>(HZPHASOR / MIN_LFO_ENV_CYCLE_TIME_S);

    uint16_t val = mux.read(mux_assignments[VO_IDX]);
    time_read.update(val);

    uint16_t processed_val = CLIP(1023 - ((time_read.getValue() * 321) >> 8) - 150, 0, 1023);
    return pgm_read_dword_near(((mode == VCO)? phasor_table : slow_phasor_table) + processed_val);
}

void Module::update_mode() {
    bool val1 = (mux.read(mux_assignments[SW_1_IDX]) > 511)? true : false;
    bool val2 = (mux.read(mux_assignments[SW_2_IDX]) > 511)? true : false;
    if (is_A) {
        if (val1 && !val2) {
            mode = ENV;
        } else if (!val1 && val2) {
            mode = VCO;
            running = true;
        } else {
            mode = LFO;
            running = true;
        }
    } else {
        if (val1 && !val2) {
            mode = VCO;
            running = true;
        } else if (!val1 && val2) {
            mode = ENV;
        } else {
            mode = LFO;
            running = true;
        }
    }
}

// if not for ratio, its for shape
uint16_t Module::get_pot_cv_val(bool for_rat) {
    uint8_t pot_idx, cv_idx;
    if (for_rat) {
        pot_idx = mux_assignments[R_PT_IDX];
        cv_idx = mux_assignments[R_CV_IDX];
    } else {
        pot_idx = mux_assignments[S_PT_IDX];
        cv_idx = mux_assignments[S_CV_IDX];
    }
    int16_t pot_val = mux.read(pot_idx);
    int16_t cv_val = mux.read(cv_idx) - 511;
    if (for_rat) {
        rat_read.update(CLIP(1023 - pot_val + cv_val, 0, 1023));
        return rat_read.getValue();
    } else {
        shp_read.update(CLIP(1023 - pot_val + cv_val, 0, 1023));
        return shp_read.getValue();
    }
}

int8_t Module::get_mod_idx_offset() {
    algo_read.update(mux.read(mux_assignments[M_CV_IDX]));
    uint16_t raw_val = algo_read.getValue();
    // if (is_A) Serial.println(raw_val >> 7);
    return static_cast<int8_t>(raw_val >> 7);

}


uint16_t A_mux_assignments[] = {R_CV_A, R_POT_A, S_CV_A, S_POT_A, M_CV_A, SW_1_A, SW_2_A, EXP_CV_A};
uint16_t B_mux_assignments[] = {R_CV_B, R_POT_B, S_CV_B, S_POT_B, M_CV_B, SW_1_B, SW_2_B, EXP_CV_B};

Module::Module(int time_pin, int mux_pin, bool _is_A): 
    mode(VCO),
    mux(admux::Pin(mux_pin, INPUT, admux::PinType::Analog), admux::Pinset(MUX_S0, MUX_S1, MUX_S2)),
    lin_time_pin(time_pin),
    rat_read(0, true),
    shp_read(0, true),
    algo_read(0, true),
    time_read(0, true, 0.1),
    is_A(_is_A) {
        mux_assignments = (is_A)? A_mux_assignments : B_mux_assignments;
}

void Module::read_inputs_frequent(Module& other) {
    ratio = get_pot_cv_val(true);

    if (rat_read.hasChanged()) {
        upslope = calc_upslope(ratio);
        downslope = calc_downslope(ratio);
    }

    shape = get_pot_cv_val(false);

    pha = get_phasor(other);

    mod_idx = get_mod_idx_offset();
}

void Module::read_inputs_infrequent() {
    update_mode();
}

void Module::update() {
    prev_eos = end_of_cycle;
    prev_shifted_acc = shifted_acc;
    acc += pha;
    shifted_acc = acc >> 22;  // range is 0-1023

    if (prev_shifted_acc > shifted_acc) {
        if (running) {
            end_of_cycle = true;
            eos_led = true;
            EOS_start_time = update_counter;
            if (mode == ENV) running = false;
        }
    }

    if (update_counter == EOS_start_time + trig_length_in_updates) end_of_cycle = false;
    if (update_counter == EOS_start_time + trig_led_length_in_updates) eos_led = false;

    update_counter++;
}

uint16_t Module::generate() {
    val = (running)? waveform_generator(shifted_acc, shape, ratio, upslope, downslope) : 0;
    if (mode == ENV) val = (val >> 1) + HALF_Y;  // so that it goes from 0 to top instead of -top to top
    if (ratio > shifted_acc) acc_by_val[val >> 6] = acc;  // for retriggering of envelopes
    return val;
}

void Module::reset() {
    if (mode == ENV && ratio <= shifted_acc) {
        acc = acc_by_val[val >> 6];
        shifted_acc = acc >> 22;
        prev_shifted_acc = shifted_acc;
    } else {
        acc = 0;
        shifted_acc = 0;
        prev_shifted_acc = 0;
    }
}

void Module::print_mode() {
    if (mode == ENV) Serial.println("ENV");
    else if (mode == VCO) Serial.println("VCO");
    else Serial.println("LFO");
}