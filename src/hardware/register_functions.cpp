#include "register_functions.h"

void setup_timers() {
    GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC |          // Improve duty cycle
                    GCLK_GENCTRL_GENEN |        // Enable generic clock gen
                    GCLK_GENCTRL_SRC_DFLL48M |  // Select 48MHz as source
                    GCLK_GENCTRL_ID(4);         // Select GCLK4
    while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

    // Set clock divider of 1 to generic clock generator 4
    GCLK->GENDIV.reg = GCLK_GENDIV_DIV(1) |         // Divide 48 MHz by 1
                        GCLK_GENDIV_ID(4);           // Apply to GCLK4 4
    while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

    // Enable GCLK4 and connect it to TCC0 and TCC1
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |        // Enable generic clock
                        GCLK_CLKCTRL_GEN_GCLK4 |    // Select GCLK4
                        GCLK_CLKCTRL_ID_TCC0_TCC1;  // Feed GCLK4 to TCC0/1
    while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

    // Divide counter by 1 giving 48 MHz (20.83 ns) on each TCC0 tick
    TCC0->CTRLA.reg |= TCC_CTRLA_PRESCALER(TCC_CTRLA_PRESCALER_DIV1_Val);

    // Use "Normal PWM" (single-slope PWM): count up to PER, match on CC[n]
    TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;         // Select NPWM as waveform
    while (TCC0->SYNCBUSY.bit.WAVE);                // Wait for synchronization

    // Set the period (the number to count to (TOP) before resetting timer)
    TCC0->PER.reg = 1023;
    while (TCC0->SYNCBUSY.bit.PER);

    // Set PWM signal to output 50% duty cycle
    // n for CC[n] is determined by n = x % 4 where x is from WO[x]
    TCC0->CC[2].reg = 511;
    while (TCC0->SYNCBUSY.bit.CC2);

    // Configure PA18 (D10 on Arduino Zero) to be output
    PORT->Group[PORTA].DIRSET.reg = PORT_PA18;      // Set pin as output
    PORT->Group[PORTA].OUTCLR.reg = PORT_PA18;      // Set pin to low
    
    // other ones to configure are PA19, PA04, PA05
    PORT->Group[PORTA].DIRSET.reg = PORT_PA19;
    PORT->Group[PORTA].OUTCLR.reg = PORT_PA19;
    PORT->Group[PORTA].DIRSET.reg = PORT_PA04;
    PORT->Group[PORTA].OUTCLR.reg = PORT_PA04;
    PORT->Group[PORTA].DIRSET.reg = PORT_PA05;
    PORT->Group[PORTA].OUTCLR.reg = PORT_PA05;

    // Enable the port multiplexer
    PORT->Group[PORTA].PINCFG[18].reg |= PORT_PINCFG_PMUXEN;
    PORT->Group[PORTA].PINCFG[19].reg |= PORT_PINCFG_PMUXEN;
    PORT->Group[PORTA].PINCFG[04].reg |= PORT_PINCFG_PMUXEN;
    PORT->Group[PORTA].PINCFG[05].reg |= PORT_PINCFG_PMUXEN;

    // Connect TCC0 timer to PA18. Function F is TCC0/WO[2] for PA18.
    // Odd pin num (2*n + 1): use PMUXO
    // Even pin num (2*n): use PMUXE
    // PORT->Group[PORTA].PMUX[9].reg = PORT_PMUX_PMUXE_F;  // PA18
    // PORT->Group[PORTA].PMUX[9].reg = PORT_PMUX_PMUXO_F;  // PA19
    // PORT->Group[PORTA].PMUX[2].reg = PORT_PMUX_PMUXE_E;  // PA04
    // PORT->Group[PORTA].PMUX[2].reg = PORT_PMUX_PMUXO_E;  // PA05


    PORT->Group[g_APinDescription[10].ulPort].PMUX[g_APinDescription[10].ulPin >> 1].reg |= PORT_PMUX_PMUXE_F;  // D10 is on PA18 = even   WO[2]
    PORT->Group[g_APinDescription[12].ulPort].PMUX[g_APinDescription[12].ulPin >> 1].reg |= PORT_PMUX_PMUXO_F; // D12 is on PA19 = odd  WO[3]
    PORT->Group[g_APinDescription[A3].ulPort].PMUX[g_APinDescription[A3].ulPin >> 1].reg |= PORT_PMUX_PMUXE_E;  // A3 is on PA04 = even WO[0]
    PORT->Group[g_APinDescription[A4].ulPort].PMUX[g_APinDescription[A4].ulPin >> 1].reg |= PORT_PMUX_PMUXO_E; // A4 is on PA05 = odd WO[1]


    TCC0->INTENSET.reg = 0;
    TCC0->INTENSET.bit.CNT = 1; // enable count interrupt
    TCC0->INTENSET.bit.MC0 = 0;

    // enable interrupts and being output
    NVIC_EnableIRQ(TCC0_IRQn);
    TCC0->CTRLA.reg |= TCC_CTRLA_ENABLE;
}