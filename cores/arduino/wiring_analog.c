#include "wiring_analog.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A0 - PC0 - ADC1_IN10
 * A1 - PC1 - ADC1_IN11
 * A2 - PC2 - ADC1_IN12
 * A3 - PC3 - ADC1_IN13
 * A4 - PC4 - ADC1_IN14
 * A5 - PC5 - ADC1_IN15
 */
uint32_t analogRead(uint32_t pin)
{
    // system clocks configuration
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_ADC1);
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);

    // GPIO configuration
    gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, analogPinToBit(pin));

    // ADC configuration
    adc_mode_config(ADC1, ADC_MODE_FREE);
    adc_data_alignment_config(ADC1, ADC_DATAALIGN_RIGHT);
    adc_channel_length_config(ADC1, ADC_REGULAR_CHANNEL, 1U);
    adc_external_trigger_source_config(ADC1, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_INSERTED_NONE);
    adc_external_trigger_config(ADC1, ADC_REGULAR_CHANNEL, ENABLE);

    adc_enable(ADC1);
    delay_1ms(1);
    adc_calibration_enable(ADC1);

    // read ADC data and return
    adc_regular_channel_config(ADC1, 0, analogPinToAdcChannel(pin), ADC_SAMPLETIME_7POINT5);
    adc_software_trigger_enable(ADC1, ADC_REGULAR_CHANNEL);

    //while (!adc_flag_get(ADC1, ADC_FLAG_EOC));
    adc_flag_clear(ADC1, ADC_FLAG_EOC);

    return (uint32_t)(adc_regular_data_read(ADC1) / 4);
}

void analogWrite(uint32_t awPin, uint32_t awVal)
{
    if (awVal < 0) { return; }
    if (awVal > 255) {
        awVal = 255;
    }

    if (awPin == 13) {
        rcu_periph_clock_enable(RCU_GPIOA);
        rcu_periph_clock_enable(RCU_DAC);
        gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_5);

        dac_deinit();
        /* configure the DAC1 */
        dac_trigger_source_config(DAC1, DAC_TRIGGER_SOFTWARE);
        dac_trigger_enable(DAC1);
        dac_wave_mode_config(DAC1, DAC_WAVE_DISABLE);
        dac_output_buffer_enable(DAC1);
        /* enable DAC1 */
        dac_enable(DAC1);

        dac_data_set(DAC1, DAC_ALIGN_8B_R, (uint8_t)awVal);
        dac_software_trigger_enable(DAC1);

        return;
    }

    // system clocks configuration
    rcu_periph_clock_enable(analogPinToPortRCU(awPin));
    rcu_periph_clock_enable(RCU_AF);
    // GPIO configuration
    gpio_init(analogPinToPort(awPin), GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, analogPinToBit(awPin));

    // Timer configuration
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(analogPinToTimerRCU(awPin));

    timer_deinit(analogPinToTimer(awPin));
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER configuration */
    timer_initpara.prescaler         = 107;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 9999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(analogPinToTimer(awPin), &timer_initpara);

    /* initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* CH configuration in PWM mode */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(analogPinToTimer(awPin), analogPinToTimerChannel(awPin), &timer_ocinitpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(analogPinToTimer(awPin));
    /* auto-reload preload enable */
    timer_enable(analogPinToTimer(awPin));

    /* CH configuration in PWM mode1, duty cycle: (awVal / 255) * 100% */
    timer_channel_output_pulse_value_config(analogPinToTimer(awPin), analogPinToTimerChannel(awPin), awVal * 9999 / 255);
    timer_channel_output_mode_config(analogPinToTimer(awPin), analogPinToTimerChannel(awPin), TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(analogPinToTimer(awPin), analogPinToTimerChannel(awPin), TIMER_OC_SHADOW_DISABLE);
}


#ifdef __cplusplus
}
#endif