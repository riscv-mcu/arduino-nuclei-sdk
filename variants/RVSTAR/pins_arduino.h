#ifndef _PINS_ARDUINO_H_
#define _PINS_ARDUINO_H_
#include <stdbool.h>
#include <stdlib.h>
#include "nuclei_sdk_hal.h"


#ifdef __cplusplus
extern "C" {
#endif

/* LEDs */
#define LED_BUILTIN  LED_GREEN
#define LED_RED      23
#define LED_GREEN    22
#define LED_BLUE     24

#define PIN_SPI_SS    (10)
#define PIN_SPI_MOSI  (11)
#define PIN_SPI_MISO  (12)
#define PIN_SPI_SCK   (13)

static const uint8_t SS   = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

#define PIN_WIRE_SDA        (20)
#define PIN_WIRE_SCL        (21)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

/* Pin aliases: these give the GPIO port/bit for each pin as an
 * enum. These are optional, but recommended. They make it easier to
 * write code using low-level GPIO functionality. */
enum {
    PC11,  // 0,
    PC10,  // 1,
    PC8,   // 2,
    PC9,   // 3,
    PA8,   // 4,
    PB1,   // 5, TIMER2_CH3
    PB0,   // 6, TIMER2_CH2
    PB5,   // 7,
    PB6,   // 8, TIMER3_CH0
    PB8,   // 9, TIMER3_CH2
    PB9,   // 10, TIMER3_CH3
    PA7,   // 11, TIMER2_CH1
    PA6,   // 12, TIMER2_CH0
    PA5,   // 13, DAC1
    PC0,   // 14, ADC1_IN10
    PC1,   // 15, ADC1_IN11
    PC2,   // 16, ADC1_IN12
    PC3,   // 17, ADC1_IN13
    PC4,   // 18, ADC1_IN14
    PC5,   // 19, ADC1_IN15
    PB11,  // 20, SDA
    PB10,  // 21, SCL
    PA1,   // 22, LED_G
    PA2,   // 23, LED_R
    PA3    // 24, LED_B
};

// You can use Ax(x=1,2,3,4,5) or digital Pin numbers to use Analog I/O functions
enum {
    A0 = 14,
    A1,
    A2,
    A3,
    A4,
    A5
};

typedef struct _gpio_dev_t {
    uint32_t gpio_port;
    rcu_periph_enum clk_id;
} gpio_dev_t;

typedef struct _timer_dev_t {
    uint32_t timer_dev;
    rcu_periph_enum clk_id;
} timer_dev_t;

typedef struct _adc_dev_t {
    uint32_t adc_dev;
    rcu_periph_enum clk_id;
} adc_dev_t;

typedef struct _spi_dev_t {
    uint32_t spi_dev;
    rcu_periph_enum clk_id;
} spi_dev_t;

typedef struct _gd32v_pin_dev_t {
    gpio_dev_t* gpio_device;
    uint32_t gpio_bit;
    timer_dev_t* timer_device;
    uint16_t timer_channel;
    adc_dev_t* adc_device;
    uint8_t adc_channel;
    //spi_dev_t * spi_device;
    //IRQn_Type irqn;
} gd32v_pin_dev_t;

#define BOARD_GPIO_NUM 25
extern const gd32v_pin_dev_t PIN_MAP[BOARD_GPIO_NUM];

// for digital pins in digitalRead() and digitalWrite()
#define digitalPinToPort(p)         ((PIN_MAP[p].gpio_device)->gpio_port)
#define digitalPinToPortRCU(p)      ((PIN_MAP[p].gpio_device)->clk_id)
#define digitalPinToBit(p)          (PIN_MAP[p].gpio_bit)

#define analogPinToPort(p)          ((PIN_MAP[p].gpio_device)->gpio_port)
#define analogPinToPortRCU(p)       ((PIN_MAP[p].gpio_device)->clk_id)
#define analogPinToBit(p)           (PIN_MAP[p].gpio_bit)

// using ADC in analogRead()
#define analogPinToAdc(p)           ((PIN_MAP[p].adc_device)->adc_dev)
#define analogPinToAdcRCU(p)        ((PIN_MAP[p].adc_device)->clk_id)
#define analogPinToAdcChannel(p)    (PIN_MAP[p].adc_channel)

// using timer PWM in analogWrite()
#define analogPinToTimer(p)         ((PIN_MAP[p].timer_device)->timer_dev)
#define analogPinToTimerRCU(p)      ((PIN_MAP[p].timer_device)->clk_id)
#define analogPinToTimerChannel(p)  (PIN_MAP[p].timer_channel)


// test stuct for uart ////////////////////////
enum {
    UART_0,
    UART_1,
    UART_2,
    UART_3,
    UART_4
};

typedef struct _uart_dev_t {
    uint8_t uart_index;
    uint32_t uart_periph;
    uint32_t uart_clk;
    IRQn_Type uart_irq;
    uint8_t rx_pin;
    uint8_t tx_pin;
} uart_dev_t;

#define BOARD_UART_NUM 5
extern const uart_dev_t UART_MAP[BOARD_UART_NUM];

#define uartToPeriph(uart_x)    (UART_MAP[uart_x].uart_periph)
#define uartToRCU(uart_x)       (UART_MAP[uart_x].uart_clk)
#define uartToIrq(uart_x)       (UART_MAP[uart_x].uart_irq)
#define uartToIndex(uart_x)     (UART_MAP[uart_x].uart_index)
#define uartToRxPin(uart_x)     (UART_MAP[uart_x].rx_pin)
#define uartToTxPin(uart_x)     (UART_MAP[uart_x].tx_pin)

#define VARIANT_GPIO_OSPEED GPIO_OSPEED_50MHZ

#ifdef __cplusplus
}
#endif

#endif  // _PINS_ARDUINO_H_