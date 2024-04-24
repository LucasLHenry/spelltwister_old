#include "misc_algorithms.h"

// dimness can go from 0 to 7, where 0 is black and 7 is no dimming
uint32_t naive_dimmer(uint8_t dimness, uint32_t colour) {
    uint8_t R = static_cast<uint8_t>((colour & 0xFF0000) >> 4);
    uint8_t G = static_cast<uint8_t>((colour & 0x00FF00) >> 2);
    uint8_t B = static_cast<uint8_t>((colour & 0x0000FF) >> 0);
    return ((R >> (7 - dimness)) << 4) & ((G >> (7 - dimness)) << 2) & (B >> (7 - dimness));
}