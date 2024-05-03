#ifndef GLOBAL_SETTINGS_H
#define GLOBAL_SETTINGS_H

#define FM_ATTENUATION 6  // higher means the FM input has less of an effect on the pitch

#define TRIG_LENGTH_MS 0.25 // length of trig out signal in milliseconds
#define TRIG_LED_LENGTH_MS 75  // length of LED pulse for the trig out signal in milliseconds

#define CRUSH_AMT 13  // number of bits that the bitcrush effect removes

// how often the we write changes to the config settings to memory (shouldn't be too short so we don't wear out the flash)
#define CONFIG_WRITE_PERIOD_S 10

#endif