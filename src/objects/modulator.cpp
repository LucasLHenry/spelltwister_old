#include "modulator.h"

Modulator::Modulator(Module& a, Module& b, LedRing& ring, algo_f_ptr algo_arr[]):
    A(a),
    B(b),
    algo_ring(ring),
    ring_algos(algo_arr)
{}

uint16_t Modulator::generate_A() {
    a_val = (A.running)? ring_algos[algo_ring.a_idx](A, B) : 0;
    return a_val;
}

uint16_t Modulator::generate_B() {
    b_val = (B.running)? ring_algos[algo_ring.b_idx](B, A) : 0;
    return b_val;
}