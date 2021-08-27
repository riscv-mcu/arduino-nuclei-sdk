#include "WInterrupts.h"

typedef void (*voidFuncPtr)(void);

static void nothing(void)
{
    // do nothing
}

static volatile voidFuncPtr intFunc[2] = {
    nothing,  // D2 - PC8
    nothing   // D3 - PC9
};

void attachInterrupt(uint32_t pin, void (*userFunc)(void), int mode)
{
    switch (pin) {
        case 2:
            intFunc[0] = userFunc;
            rcu_periph_clock_enable(RCU_GPIOC);
            /* enable the AF clock */
            rcu_periph_clock_enable(RCU_AF);
            /* configure button pin as input */
            gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8);

            ECLIC_Register_IRQ(EXTI5_9_IRQn, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_LEVEL_TRIGGER, 1, 1, NULL);

            /* connect key EXTI line to key GPIO pin */
            gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_8);

            /* configure key EXTI line */
            exti_init(EXTI_8, EXTI_INTERRUPT, mode);
            exti_interrupt_flag_clear(EXTI_8);
            break;

        case 3:
            intFunc[1] = userFunc;
            rcu_periph_clock_enable(RCU_GPIOC);
            rcu_periph_clock_enable(RCU_AF);
            gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

            ECLIC_Register_IRQ(EXTI5_9_IRQn, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_LEVEL_TRIGGER, 1, 1, NULL);

            gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_9);

            exti_init(EXTI_9, EXTI_INTERRUPT, mode);
            exti_interrupt_flag_clear(EXTI_9);
            break;

        default:
            break;
    }

    interrupts();
}

void detachInterrupt(uint32_t pin)
{
    switch (pin) {
        case 2:
            exti_interrupt_disable(EXTI_8);
            intFunc[0] = nothing;
            break;

        case 3:
            exti_interrupt_disable(EXTI_9);
            intFunc[1] = nothing;
            break;

        default:
            break;
    }
}

void EXTI5_9_IRQHandler(void)
{
    if (RESET != exti_interrupt_flag_get(EXTI_8)) {
        intFunc[0]();
        exti_interrupt_flag_clear(EXTI_8);
    } else if (RESET != exti_interrupt_flag_get(EXTI_9)) {
        intFunc[1]();
        exti_interrupt_flag_clear(EXTI_9);
    }
}