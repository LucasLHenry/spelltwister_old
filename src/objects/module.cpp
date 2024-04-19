#include "module.h"

uint32_t Module::get_phasor() {
    time_read.update(mux.read(mux_assignments[VO_IDX]));
    uint32_t new_pha = pgm_read_dword_near(phasor_table + time_read.getValue());
    if (is_A) Serial.println(new_pha);
    return new_pha;
}

void Module::update_mode() {
    bool val1 = (mux.read(mux_assignments[SW_1_IDX]) > 511)? true : false;
    bool val2 = (mux.read(mux_assignments[SW_2_IDX]) > 511)? true : false;
    if (is_A) {
        if (val1 && !val2) {
            mode = ENV;
            running = false;
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
            running = false;
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


uint16_t A_mux_assignments[] = {R_CV_A, R_POT_A, S_CV_A, S_POT_A, M_CV_A, SW_1_A, SW_2_A, EXP_CV_A};
uint16_t B_mux_assignments[] = {R_CV_B, R_POT_B, S_CV_B, S_POT_B, M_CV_B, SW_1_B, SW_2_B, EXP_CV_B};

Module::Module(int time_pin, int mux_pin, bool _is_A): 
    mode(VCO),
    mux(admux::Pin(mux_pin, INPUT, admux::PinType::Analog), admux::Pinset(MUX_S0, MUX_S1, MUX_S2)),
    lin_time_pin(time_pin),
    rat_read(0, true),
    shp_read(0, true),
    time_read(0, true),
    algo_read(0, true),
    is_A(_is_A) {
    mux_assignments = (is_A)? A_mux_assignments : B_mux_assignments;
}

void Module::read_inputs() {
    /*
    have to read: 
    - mode (switch 1 and 2)
    - ratio value (ratio cv and ratio pot)
    - shape value (shape cv and shape pot)
    - time value (time pot + v/o, fm in)
    - algorithm offset

    update the parameters based on these values
    - mode -> mode
    - ratio value -> ratio, upslope, downslope
    - shape value -> shape
    - time value -> phasor
    */
    update_mode();

    ratio = get_pot_cv_val(true);
    if (rat_read.hasChanged()) {
        upslope = calc_upslope(ratio);
        downslope = calc_downslope(ratio);
    }

    shape = get_pot_cv_val(false);

    pha = get_phasor() << 4;
}

void Module::update() {
    prev_shifted_acc = shifted_acc;
    acc += pha;
    shifted_acc = acc >> 22;  // range is 0-1023
}

uint16_t Module::generate() {
    if (mode == ENV && prev_shifted_acc > shifted_acc) running = false;
    val = (running)? waveform_generator(shifted_acc, shape, ratio, upslope, downslope) : 0;
    // if (mode == ENV) val = (val >> 1) + HALF_Y;  // so that it goes from 0 to top instead of -top to top
    return val;
}