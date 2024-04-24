#include <Arduino.h>

#include "../signals/mod_algorithms.h"
#include "module.h"
#include "led_ring.h"

#ifndef MODULATOR_H
#define MODULATOR_H

class Modulator {
    Module& A;
    Module& B;
    LedRing& algo_ring;
    algo_f_ptr* ring_algos;
    public:
        Modulator(Module& a, Module& b, LedRing& ring, algo_f_ptr* algo_arr);
        uint16_t generate_A();
        uint16_t generate_B();
        uint16_t a_val, b_val;
};

#endif