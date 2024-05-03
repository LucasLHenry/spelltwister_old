#include "generator.h"

uint16_t calc_upslope(uint16_t s) {
    if (s == 0) return 0;
    return MAX_Y / s;
}

uint16_t calc_downslope(uint16_t s) {
    if (s == MAX_X) return 0;
    return MAX_Y / (MAX_X - s);
}

uint16_t asym_lin_map(uint16_t x, uint16_t low, uint16_t mid, uint16_t high) {
    if (x <= 0) return low;
    if (x < HALF_X) return (x * (mid - low) >> 9) + low;
    if (x == HALF_X) return mid;
    if (x > HALF_X) return ((x - HALF_X) * (high - mid) >> 9) + mid;
    return high;
}

uint16_t linval, expval, logval, logslope;
uint16_t waveform_generator(uint16_t shifted_acc, uint16_t shape, uint16_t ratio, uint16_t upslope, uint16_t downslope) {
    if (shifted_acc <= ratio) {
        linval = upslope * shifted_acc;
        expval = pgm_read_word_near(exptable + (linval >> 6));
        logval = pgm_read_word_near(logtable + (linval >> 6));
    } else {
        linval = downslope * (MAX_X - shifted_acc);
        expval = pgm_read_word_near(exptable + (linval >> 6));
        logval = pgm_read_word_near(logtable + (linval >> 6));
    }
    return asym_lin_map(shape, expval, linval, logval);
}