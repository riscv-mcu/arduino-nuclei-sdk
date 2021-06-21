#ifndef _WIRING_INTERRUPTS_
#define _WIRING_INTERRUPTS_

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*voidFuncPtr)(void);

extern void attachInterrupt(uint32_t pin, void (*userFunc)(void), int mode);
extern void detachInterrupt(uint32_t pin);

#ifdef __cplusplus
}
#endif

#endif /* _WIRING_INTERRUPTS_ */