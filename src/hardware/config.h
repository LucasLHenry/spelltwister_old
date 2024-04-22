#include <Arduino.h>

#include "../objects/led_ring.h"
#include "../objects/module.h"

// some settings and values are hardware-specific, and should persist between uses.
// these functions perform that operation.
// there isn't EEPROM on the SAMd21, so we use flash memory as EEPROM.
// the issue with this is that flash has a limited number of write cycles, so 
// we don't want to spam it or we'll wear the memory out. 

// the main things we want to save are:
// - position of encoder (ring.a_idx and ring.b_idx)
// - calibration info for V/O exponential converter

#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_WRITE_PERIOD_S 10

void update_values_from_config(LedRing& ring, Module& A, Module& B);
void write_encoder_to_config(LedRing& ring);
void write_calibration_to_config(Module& A, Module& B);

#endif