#include "mod_algorithms.h"

uint16_t sum(Module& main, Module& aux) {
    return (main.val + aux.val) >> 1;
}

uint16_t difference(Module& main, Module& aux) {
    return (main.val - aux.val + MAX_Y) >> 1;
}

uint16_t exculsive_or(Module& main, Module& aux) {
    uint16_t top = main.val & 0xF000;
    uint16_t bottom = (main.val & 0x0FFF) ^ (aux.val & 0x0FFF);
    return top | bottom;
}

uint16_t invert(Module& main, Module& aux) {
    return MAX_Y - main.val;
}

uint16_t double_freq(Module& main, Module& aux) {
    return waveform_generator((main.acc >> 21) % MAX_X, main.shape, main.ratio, main.upslope, main.downslope);
}

uint16_t half_freq(Module& main, Module& aux) {
    // return waveform_generator(main.acc >> 23, main.shape, main.ratio, main.upslope; main.downslope);
    return 0;
}

uint16_t rectify(Module& main, Module& aux) {
    uint32_t val = (main.val < HALF_Y)? MAX_Y - main.val : main.val;
    return (uint16_t)((val << 1) - MAX_Y);
}

uint16_t bitcrush(Module& main, Module& aux) {
    #define CRUSH_AMT 13
    return (main.val >> CRUSH_AMT) << CRUSH_AMT;
    #undef CRUSH_AMT
}


// DEEMED NOT VERY INTERESTING, SO WON'T GET INCLUDED IN FINAL RELEASE.
// also, `random` is a huge performance hit.
uint16_t noisify(Module& main, Module& aux) {
    // #define RAND_AMT 4096
    // constexpr int16_t half_rand = RAND_AMT / 2;
    // return CLIP((int16_t)main.val + random(RAND_AMT) - half_rand, 0, MAX_Y);
    return main.val;
}

uint16_t sample_rate_reduce(Module& main, Module& aux) {
    #define SRR_AMT 1
    return waveform_generator((main.shifted_acc >> SRR_AMT) << SRR_AMT, main.shape, main.ratio, main.upslope, main.downslope);
    #undef SRR_AMT
}

uint16_t wavefold(Module& main, Module& aux) {
    return 0;
}

uint16_t ratio_mod(Module& main, Module& aux) {
    return waveform_generator(main.shifted_acc, main.shape, aux.val >> 6, main.upslope, main.downslope);
}

uint16_t shape_mod(Module& main, Module& aux) {
    uint16_t shape_val = aux.val >> 6;
    return waveform_generator(main.shifted_acc, shape_val, main.ratio, calc_upslope(shape_val), calc_downslope(shape_val));
}

uint16_t gate(Module& main, Module& aux) {
    if (aux.val < HALF_Y) return 0;
    return main.val;
}

uint16_t amplitude_mod(Module& main, Module& aux) {
    return (uint16_t)((main.val * (uint64_t)aux.val) >> 16);
}

uint16_t frequency_mod(Module& main, Module& aux) {
    return 0;
}

uint16_t ring_modulate(Module& main, Module& aux) {
    return (uint16_t)((main.val * (uint64_t)aux.val) >> 16);
}