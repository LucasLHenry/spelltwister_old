#include <Arduino.h>
#include "../colours.h"
#include "generator.h"

#ifndef MISC_ALGORITHMS_H
#define MISC_ALGORITHMS_H

uint32_t naive_dimmer(uint8_t dimness, uint32_t colour);
uint32_t better_dimmer(uint8_t dimness, HSL colour_hsl);

uint32_t hsl(HSL hsl);
uint8_t hsl_convert(float c, float t1, float t2);

// HSL rgb_to_hsl(uint16_t r, uint16_t g, uint16_t b);

#endif