#include <Arduino.h>
#include "../objects/module.h"
#include "generator.h"

#ifndef MODULATION_ALGORITHMS_H
#define MODULATION_ALGORITHMS_H

typedef uint16_t (*algo_f_ptr)(Module& main, Module& aux);

uint16_t sum                 (Module& main, Module& aux);
uint16_t difference          (Module& main, Module& aux);
uint16_t exculsive_or        (Module& main, Module& aux);
uint16_t invert              (Module& main, Module& aux);
uint16_t double_freq         (Module& main, Module& aux);
uint16_t half_freq           (Module& main, Module& aux);
uint16_t rectify             (Module& main, Module& aux);
uint16_t bitcrush            (Module& main, Module& aux);
uint16_t noisify             (Module& main, Module& aux);
uint16_t sample_rate_reduce  (Module& main, Module& aux);
uint16_t wavefold            (Module& main, Module& aux);
uint16_t ratio_mod           (Module& main, Module& aux);
uint16_t shape_mod           (Module& main, Module& aux);
uint16_t gate                (Module& main, Module& aux);
uint16_t amplitude_mod       (Module& main, Module& aux);
uint16_t frequency_mod       (Module& main, Module& aux);
uint16_t ring_modulate       (Module& main, Module& aux);

#endif