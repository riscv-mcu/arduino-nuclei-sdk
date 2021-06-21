#include "pins_arduino.h"

gpio_dev_t gpioa = {
    .gpio_port = GPIOA,
    .clk_id = RCU_GPIOA,
};

gpio_dev_t gpiob = {
    .gpio_port = GPIOB,
    .clk_id = RCU_GPIOB,
};

gpio_dev_t gpioc = {
    .gpio_port = GPIOC,
    .clk_id = RCU_GPIOC,
};


timer_dev_t timer0 = {
    .timer_dev = TIMER0,
    .clk_id = RCU_TIMER0,
};

timer_dev_t timer1 = {
    .timer_dev = TIMER1,
    .clk_id = RCU_TIMER1,
};

timer_dev_t timer2 = {
    .timer_dev = TIMER2,
    .clk_id = RCU_TIMER2,
};

timer_dev_t timer3 = {
    .timer_dev = TIMER3,
    .clk_id = RCU_TIMER3,
};

timer_dev_t timer4 = {
    .timer_dev = TIMER4,
    .clk_id = RCU_TIMER4,
};

adc_dev_t adc0 = {
    .adc_dev = ADC0,
    .clk_id = RCU_ADC0,
};

adc_dev_t adc1 = {
    .adc_dev = ADC1,
    .clk_id = RCU_ADC1,
};


const gd32v_pin_dev_t PIN_MAP[BOARD_GPIO_NUM] = {
/*
 * gpio_dev    *gpio_device;    GPIO device 
 * uint32      gpio_bit;        Pin's GPIO port bit. 
 * timer_dev   *timer_device;   Pin's timer device, if any.
 * uint16      timer_channel;   Timer channel, or 0 if none. 
 * adc_dev     *adc_device;     ADC device, if any.
 * uint8       adc_channel;     Pin ADC channel, or ADCx if none. 
 * todo: IRQn_Type irqn
*/
    {&gpioc, GPIO_PIN_11, 0, 0, 0, 0}, // 0  - PC11 - UART3_RX
    {&gpioc, GPIO_PIN_10, 0, 0, 0, 0}, // 1  - PC10 - UART3_TX
    {&gpioc, GPIO_PIN_8, 0, 0, 0, 0}, // 2  - PC8
    {&gpioc, GPIO_PIN_9, 0, 0, 0, 0}, // 3  - PC9
    {&gpioa, GPIO_PIN_8, 0, 0, 0, 0}, // 4  - PA8
    {&gpiob, GPIO_PIN_1, &timer2, TIMER_CH_3, 0, 0}, // 5  - PB1 - TIMER2_CH3
    {&gpiob, GPIO_PIN_0, &timer2, TIMER_CH_2, 0, 0}, // 6  - PB0 - TIMER2_CH2
    {&gpiob, GPIO_PIN_5, 0, 0, 0, 0}, // 7  - PB5
    {&gpiob, GPIO_PIN_6, &timer3, TIMER_CH_0, 0, 0}, // 8  - PB6 - TIMER3_CH0
    {&gpiob, GPIO_PIN_8, &timer3, TIMER_CH_2, 0, 0}, // 9  - PB8 - TIMER3_CH2
    {&gpiob, GPIO_PIN_9, &timer3, TIMER_CH_3, 0, 0}, // 10 - PB9 - TIMER3_CH3
    {&gpioa, GPIO_PIN_7, &timer2, TIMER_CH_1, 0, 0}, // 11 - PA7 - TIMER2_CH1
    {&gpioa, GPIO_PIN_6, &timer2, TIMER_CH_0, 0, 0}, // 12 - PA6 - TIMER2_CH0
    {&gpioa, GPIO_PIN_5, 0, 0, 0, 0}, // 13 - PA5
    {&gpioc, GPIO_PIN_0, 0, 0, &adc1, ADC_CHANNEL_10}, // 14 - PC0 - A0 - ADC1_IN10
    {&gpioc, GPIO_PIN_1, 0, 0, &adc1, ADC_CHANNEL_11}, // 15 - PC1 - A1 - ADC1_IN11
    {&gpioc, GPIO_PIN_2, 0, 0, &adc1, ADC_CHANNEL_12}, // 16 - PC2 - A2 - ADC1_IN12
    {&gpioc, GPIO_PIN_3, 0, 0, &adc1, ADC_CHANNEL_13}, // 17 - PC3 - A3 - ADC1_IN13
    {&gpioc, GPIO_PIN_4, 0, 0, &adc1, ADC_CHANNEL_14}, // 18 - PC4 - A4 - ADC1_IN14
    {&gpioc, GPIO_PIN_5, 0, 0, &adc1, ADC_CHANNEL_15}, // 19 - PC5 - A5 - ADC1_IN15
    {&gpiob, GPIO_PIN_11, 0, 0, 0, 0}, // 20 - PB11 - SDA
    {&gpiob, GPIO_PIN_10, 0, 0, 0, 0}, // 21 - PB10 - SCL
    {&gpioa, GPIO_PIN_1, 0, 0, 0, 0},  // 22 - LED_G  - PA1
    {&gpioa, GPIO_PIN_2, 0, 0, 0, 0},  // 23 - LED_R  - PA2
    {&gpioa, GPIO_PIN_3, 0, 0, 0, 0}   // 24 - LED_B  - PA3
};

const uart_dev_t UART_MAP[BOARD_UART_NUM] = {
    {0, USART0, RCU_USART0, 0, 0, 0},
    {1, USART1, RCU_USART1, 0, 0, 0},
    {2, USART2, RCU_USART2, 0, 0, 0},
    {3, UART3, RCU_UART3, UART3_IRQn, 0, 1},
    {4, UART4, RCU_UART4, UART4_IRQn, 0, 0}
};