#include "tone.h"

#define MAX_FREQ  65535

#ifdef __cplusplus
extern "C" {
#endif

void tone(uint8_t pin, unsigned int freq)
{
    if(analogPinToTimer(pin) != 0 && freq >= 0)
    {   
        // system clocks configuration
        rcu_periph_clock_enable(analogPinToPortRCU(pin));
        rcu_periph_clock_enable(RCU_AF);
        // GPIO configuration
        gpio_init(analogPinToPort(pin), GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, analogPinToBit(pin));
        
        // Timer configuration
        timer_oc_parameter_struct timer_ocinitpara;
        timer_parameter_struct timer_initpara;

        rcu_periph_clock_enable(analogPinToTimerRCU(pin));

        timer_deinit(analogPinToTimer(pin));
        /* initialize TIMER init parameter struct */
        timer_struct_para_init(&timer_initpara);
        /* TIMER2 configuration */
        timer_initpara.prescaler         = 107;
        timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
        timer_initpara.counterdirection  = TIMER_COUNTER_UP;
        timer_initpara.period            = 1000000/freq;
        timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
        timer_initpara.repetitioncounter = 0;
        timer_init(analogPinToTimer(pin), &timer_initpara);

        /* initialize TIMER channel output parameter struct */
        timer_channel_output_struct_para_init(&timer_ocinitpara);
        /* CH configuration in PWM mode */
        timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
        timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
        timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
        timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
        timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
        timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

        timer_channel_output_config(analogPinToTimer(pin), analogPinToTimerChannel(pin), &timer_ocinitpara);


        /* auto-reload preload enable */
        timer_auto_reload_shadow_enable(analogPinToTimer(pin));
        /* auto-reload preload enable */
        timer_enable(analogPinToTimer(pin));

        /* CH configuration in PWM mode1, duty cycle 50% */
        timer_channel_output_pulse_value_config(analogPinToTimer(pin),analogPinToTimerChannel(pin), 500000/freq);
        timer_channel_output_mode_config(analogPinToTimer(pin),analogPinToTimerChannel(pin),TIMER_OC_MODE_PWM0);
        timer_channel_output_shadow_config(analogPinToTimer(pin),analogPinToTimerChannel(pin),TIMER_OC_SHADOW_DISABLE);
    }
}

void noTone(uint8_t pin)
{
    timer_deinit(analogPinToTimer(pin));
}

#ifdef __cplusplus
}
#endif