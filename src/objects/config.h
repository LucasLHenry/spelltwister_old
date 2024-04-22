#include <Arduino.h>
#include <FlashAsEEPROM_SAMD.h>

// some settings and values are hardware-specific, and should persist between uses.
// these functions perform that operation.
// there isn't EEPROM on the SAMd21, so we use flash memory as EEPROM.
// the issue with this is that flash has a limited number of write cycles, so 
// we don't want to spam it or we'll wear the memory out. 

#ifndef CONFIG_H
#define CONFIG_H

#endif