#include <Arduino.h>
#include <Mux.h>
#include <ResponsiveAnalogRead.h>
#include <avr/pgmspace.h>

#include "../hardware/pins.h"
#include "../signals/generator.h"
#include "../tables/phasors.h"
#include "../tables/slow_phasors.h"

#ifndef MODULE_CLASS_H
#define MODULE_CLASS_H

#define HZPHASOR 91183 //phasor value for 1 hz.

#define TRIG_LENGTH_MS 0.25
#define TRIG_LED_LENGTH_MS 75
// ms * 1s/1000ms * 48000 updates/s
constexpr uint64_t trig_length_in_updates = static_cast<uint64_t>(TRIG_LENGTH_MS * 48);
constexpr uint64_t trig_led_length_in_updates = static_cast<uint64_t>(TRIG_LED_LENGTH_MS * 48);

enum Mode {VCO, LFO, ENV};

class Module {
    uint16_t raw_shape_pot, raw_ratio_pot, raw_shape_cv, raw_ratio_cv, raw_lin_time, raw_exp_time, raw_mod;
    uint64_t update_counter;
    uint64_t EOS_start_time;
    int lin_time_pin, mux_pin;
    bool is_A;
    uint16_t* mux_assignments;
    uint32_t get_phasor(Module& other);
    void update_mode();
    uint16_t get_pot_cv_val(bool for_rat);
    int8_t get_mod_idx_offset();
    ResponsiveAnalogRead rat_read, shp_read, time_read, algo_read;
    admux::Mux mux;
    uint32_t prev_shifted_acc;
    uint32_t acc_by_val[1024];
    public:
        Mode mode;
        bool follow;
        uint16_t shifted_acc;
        uint32_t acc, pha;
        uint16_t upslope, downslope;
        uint16_t ratio, shape;
        bool running;
        Module(int time_pin, int mux_pin, bool is_A);
        void read_inputs_frequent(Module& other); // for things that need to be updated often
        void read_inputs_infrequent(); // for things that don't need to be updated as often
        void update();
        uint16_t generate();
        void reset();
        uint16_t val;
        uint16_t algo_offset;
        void print_info(bool verbose);
        uint16_t vo_offset, vo_scale;
        bool end_of_cycle, prev_eos;
        bool eos_led;
        int8_t mod_idx;
};

// order for mux_assignemnts is ratio cv, ratio pot, shape cv, shape pot, algo cv, switch 1, switch 2, exp time cv
#define R_CV_IDX 0
#define R_PT_IDX 1
#define S_CV_IDX 2
#define S_PT_IDX 3
#define M_CV_IDX 4
#define SW_1_IDX 5
#define SW_2_IDX 6
#define VO_IDX   7

#endif