#include <Arduino.h>
#include <avr/pgmspace.h>
#include "../tables/exptable.h"
#include "../tables/logtable.h"


#ifndef GENERATOR_FUNC_H
#define GENERATOR_FUNC_H

#define MAX_X  1023
#define MAX_Y  65535
#define HALF_X 511
#define HALF_Y 32767

#define MAX(a, b) (((a) > (b))? (a) : (b))
#define MIN(a, b) (((a) < (b))? (a) : (b))
#define CLIP(x, a, b) MAX(MIN(x, b), a)

uint16_t calc_upslope(uint16_t x);
uint16_t calc_downslope(uint16_t x);
uint16_t asym_lin_map(uint16_t x, uint16_t low, uint16_t mid, uint16_t high);

extern uint16_t linval, expval, logval;

uint16_t waveform_generator(uint16_t shifted_acc, uint16_t shape, uint16_t ratio, uint16_t upslope, uint16_t downslope);

#endif