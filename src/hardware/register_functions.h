#include <Arduino.h>

#ifndef REGISTER_FUNCTIONS_H
#define REGISTER_FUNCTIONS_H

#define A_PRI_REG  REG_TCC0_CCB1
#define A_SEC_REG  REG_TCC0_CCB0
#define A_TRIG_REG PORT_PA08
#define B_PRI_REG  REG_TCC0_CCB2
#define B_SEC_REG  REG_TCC0_CCB3
#define B_TRIG_REG PORT_PA20

void setup_timers();
void digitalWriteDirect(int PIN, boolean val);
void set_cpu_frequency(uint8_t frequency);

#endif