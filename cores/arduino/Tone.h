#ifndef _TONE_H_
#define _TONE_H_

#include "Arduino.h"

extern void tone(uint8_t pin, unsigned int freq);
extern void noTone(uint8_t pin);

#endif // _TONE_H_