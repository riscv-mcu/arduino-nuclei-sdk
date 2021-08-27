#include "wiring_digital.h"

#ifdef __cplusplus
extern "C" {
#endif

void pinMode(uint32_t pin, uint32_t mode)
{
    if(pin >= BOARD_GPIO_NUM || pin < 0)
        return;

    switch (mode)
    {
        case INPUT:
            rcu_periph_clock_enable(digitalPinToPortRCU(pin));
            gpio_init(digitalPinToPort(pin), GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, digitalPinToBit(pin));
            break;
        case OUTPUT:
            rcu_periph_clock_enable(digitalPinToPortRCU(pin));
            gpio_init(digitalPinToPort(pin), GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, digitalPinToBit(pin));
            break;
        case INPUT_PULLUP: 
            rcu_periph_clock_enable(digitalPinToPortRCU(pin));
            gpio_init(digitalPinToPort(pin), GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, digitalPinToBit(pin));
            break;
        default:
            break;
    }
}

void digitalWrite(uint32_t pin, uint32_t val)
{
    if(pin >= BOARD_GPIO_NUM || pin < 0)
        return;
    
    switch (val)
    {
        case HIGH:
            GPIO_BC(digitalPinToPort(pin)) = digitalPinToBit(pin);
            break; 
        case LOW: 
            GPIO_BOP(digitalPinToPort(pin)) = digitalPinToBit(pin);
            break;
        default:
            break;
    }
}

int digitalRead(uint32_t pin)
{
    if(pin >= BOARD_GPIO_NUM || pin < 0)
        return LOW;

    return gpio_input_bit_get(digitalPinToPort(pin), digitalPinToBit(pin));
}


#ifdef __cplusplus
}
#endif