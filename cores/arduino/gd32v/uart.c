#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif

// need a global variable to store an addrress related to "&_serial"
static serial_t *obj[BOARD_UART_NUM];

void uart_init(serial_t *obj_s, uint32_t baudrate, uint32_t databits, uint32_t parity, uint32_t stopbits)
{
    obj[obj_s->index] = obj_s;
    rcu_periph_clock_enable(digitalPinToPort(obj_s->rx_pin));
    rcu_periph_clock_enable(digitalPinToPort(obj_s->tx_pin));
    rcu_periph_clock_enable(uartToRCU(obj_s->index));
    gpio_init(digitalPinToPort(obj_s->rx_pin), GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, digitalPinToBit(obj_s->rx_pin));
    gpio_init(digitalPinToPort(obj_s->tx_pin), GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, digitalPinToBit(obj_s->tx_pin));

    usart_deinit(obj_s->uart_periph);
    usart_baudrate_set(obj_s->uart_periph, baudrate);
    usart_word_length_set(obj_s->uart_periph, databits);
    usart_stop_bit_set(obj_s->uart_periph, stopbits);
    usart_parity_config(obj_s->uart_periph, parity);
    usart_hardware_flow_rts_config(obj_s->uart_periph, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(obj_s->uart_periph, USART_CTS_DISABLE);
    usart_receive_config(obj_s->uart_periph, USART_RECEIVE_ENABLE);
    usart_transmit_config(obj_s->uart_periph, USART_TRANSMIT_ENABLE);
    usart_enable(obj_s->uart_periph);
}

void uart_deinit(serial_t *obj_s)
{
    usart_deinit(obj_s->uart_periph);
}

void uart_write(serial_t *obj_s, uint8_t data, uint16_t size)
{
    usart_data_transmit(obj_s->uart_periph, data);
}

int uart_getc(serial_t *obj_s, unsigned char *c)
{
    return usart_data_receive(obj_s->uart_periph);
}

void UART3_IRQHandler(void)
{
    // get a char from the register to rx buffer and clear the flag
    if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE)) {
        unsigned char c = usart_data_receive(UART3);
        uint8_t i = (unsigned int)(obj[3]->rx_head + 1) % 64;
        
        if(i != obj[3]->rx_tail) {
            obj[3]->rx_buff[obj[3]->rx_head] = c;
            obj[3]->rx_head = i;
        }
    }
    // send a char from the tx buffer
    if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_TBE)){
        unsigned char c = obj[3]->tx_buff[obj[3]->tx_tail];
        obj[3]->tx_tail = ( obj[3]->tx_tail + 1 ) % 64;

        usart_data_transmit(UART3, c);

        if(obj[3]->tx_head == obj[3]->tx_tail) {
            usart_interrupt_disable(UART3, USART_FLAG_TBE);
        }
    }
}

void UART4_IRQHandler(void)
{
    // get a char from the register to rx buffer and clear the flag
    if(RESET != usart_interrupt_flag_get(UART4, USART_INT_FLAG_RBNE)) {
        unsigned char c = usart_data_receive(UART4);
        uint8_t i = (unsigned int)(obj[4]->rx_head + 1) % 64;
        
        if(i != obj[4]->rx_tail) {
            obj[4]->rx_buff[obj[4]->rx_head] = c;
            obj[4]->rx_head = i;
        }
    }
    // send a char from the tx buffer
    if(RESET != usart_interrupt_flag_get(UART4, USART_INT_FLAG_TBE)){
        unsigned char c = obj[4]->tx_buff[obj[4]->tx_tail];
        obj[4]->tx_tail = ( obj[4]->tx_tail + 1 ) % 64;

        usart_data_transmit(UART4, c);

        if(obj[4]->tx_head == obj[4]->tx_tail) {
            usart_interrupt_disable(UART4, USART_FLAG_TBE);
        }
    }
}

#ifdef __cplusplus
}
#endif