#include <Arduino.h>

#ifndef REGISTER_FUNCTIONS_H
#define REGISTER_FUNCTIONS_H

#define A_PRI_REG REG_TCC0_CCB1
#define A_SEC_REG REG_TCC0_CCB0
#define B_PRI_REG REG_TCC0_CCB2
#define B_SEC_REG REG_TCC0_CCB3

void setup_timers();


#endif