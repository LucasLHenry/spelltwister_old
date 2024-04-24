#include "modulator.h"

Modulator::Modulator(Module& a, Module& b, LedRing& ring, algo_f_ptr algo_arr[]):
    A(a),
    B(b),
    algo_ring(ring),
    ring_algos(algo_arr)
{}

uint16_t Modulator::generate_A() {
    a_val = ring_algos[algo_ring.a_idx](A, B);
    return a_val;
}

uint16_t Modulator::generate_B() {
    b_val = ring_algos[algo_ring.b_idx](B, A);
    return b_val;
}