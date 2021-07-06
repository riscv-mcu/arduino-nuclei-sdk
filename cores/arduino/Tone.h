#ifndef _TONE_H_
#define _TONE_H_

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void tone(uint8_t pin, unsigned int freq);
extern void noTone(uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif // _TONE_H_