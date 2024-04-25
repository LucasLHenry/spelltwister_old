#include <Arduino.h>

#ifndef COLOURS_H
#define COLOURS_H

#define ORANGE    0xFF7000
#define RED       0xFF0000
#define WHITE     0xFFFFFF
#define BLACK     0x000000
#define TURQUOISE 0x00FFFF
#define PINK      0xFF00FF
#define PLORANGE  0xBF3880
#define PURPLE    0x7F00FF
#define BLUE      0x0000FF

typedef struct HSL {
    uint32_t h;
    uint8_t s;
    uint8_t l;
} HSL;

#define RED_HSL (HSL){0, 100, 50}
#define BLUE_HSL (HSL){240, 100, 50}

#endif