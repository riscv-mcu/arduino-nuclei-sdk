#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif


uint32_t millis(void)
{
    return (uint32_t)(SysTimer_GetLoadValue() * (4000.F / SystemCoreClock));
}


uint32_t micros(void)
{
    return (uint32_t)(SysTimer_GetLoadValue() * (4000000.F / SystemCoreClock));
}


void delay(uint32_t ms)
{
    if (ms >= 0)
    {
        delay_1ms(ms);
    }
}

void delayMicroseconds(uint32_t us)
{
    uint64_t start_mtime, delta_mtime;
    uint64_t delay_ticks = (SOC_TIMER_FREQ * (uint64_t)us) / 1000000;

    start_mtime = SysTimer_GetLoadValue();

    do {
        delta_mtime = SysTimer_GetLoadValue() - start_mtime;
    } while (delta_mtime < delay_ticks);  
}


#ifdef __cplusplus
} // extern "C"
#endif