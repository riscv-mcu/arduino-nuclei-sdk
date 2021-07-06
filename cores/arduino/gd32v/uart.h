#ifndef _UART_H_
#define _UART_H_

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _serial_t {
    uint32_t uart_periph;
    uint32_t uart_clk;
    IRQn_Type uart_irq;
    uint32_t rx_pin;
    uint32_t tx_pin;
    uint8_t index;
    uint8_t recv;
    uint8_t *rx_buff;
    uint8_t *tx_buff;
    uint16_t rx_tail;
    uint16_t tx_head;
    volatile uint16_t rx_head;
    volatile uint16_t tx_tail;
}serial_t;

void uart_init(serial_t *obj_s, uint32_t baudrate, uint32_t databits, uint32_t parity, uint32_t stopbits);
void uart_deinit(serial_t *obj_s);

void uart_write(serial_t *obj_s, uint8_t data, uint16_t size);
int uart_getc(serial_t *obj_s, unsigned char *c);

void UART3_IRQHandler(void);
void UART4_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif