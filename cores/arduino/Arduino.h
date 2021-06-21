#ifndef Arduino_h
#define Arduino_h

#include "wiring.h"
#include "pins_arduino.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cpp

extern void setup();
extern void loop();

#ifdef __cplusplus
}  // extern "C"
#endif  // __cpp

#endif // Arduino_h