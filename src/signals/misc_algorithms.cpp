#include "misc_algorithms.h"

// dimness can go from 0 to 7, where 0 is black and 7 is no dimming
uint32_t naive_dimmer(uint8_t dimness, uint32_t colour) {
    uint8_t R = static_cast<uint8_t>((colour & 0xFF0000) >> 4);
    uint8_t G = static_cast<uint8_t>((colour & 0x00FF00) >> 2);
    uint8_t B = static_cast<uint8_t>((colour & 0x0000FF) >> 0);
    return ((R >> (7 - dimness)) << 4) & ((G >> (7 - dimness)) << 2) & (B >> (7 - dimness));
}

uint32_t better_dimmer(uint8_t dimness, HSL colour_hsl) {
    colour_hsl.l = dimness;
    return hsl(colour_hsl);
}

uint32_t hsl(HSL hsl) {
    float h, s, l, t1, t2, tr, tg, tb;
    uint8_t r, g, b;

    h = (hsl.h % 360) / 360.0;
    s = CLIP(hsl.s, 0, 100) / 100.0;
    l = CLIP(hsl.l, 0, 100) / 100.0;

    if ( s == 0 ) { 
        r = g = b = 255 * l;
        return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
    } 

    if ( l < 0.5 ) t1 = l * (1.0 + s);
    else t1 = l + s - l * s;

    t2 = 2 * l - t1;
    tr = h + 1/3.0;
    tg = h;
    tb = h - 1/3.0;

    r = hsl_convert(tr, t1, t2);
    g = hsl_convert(tg, t1, t2);
    b = hsl_convert(tb, t1, t2);

    // NeoPixel packed RGB color
    return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}


uint8_t hsl_convert(float c, float t1, float t2) {
    if ( c < 0 ) c+=1; 
    else if ( c > 1 ) c-=1;

    if ( 6 * c < 1 ) c = t2 + ( t1 - t2 ) * 6 * c;
    else if ( 2 * c < 1 ) c = t1;
    else if ( 3 * c < 2 ) c = t2 + ( t1 - t2 ) * ( 2/3.0 - c ) * 6;
    else c = t2;

    return (uint8_t)(c*255); 
}