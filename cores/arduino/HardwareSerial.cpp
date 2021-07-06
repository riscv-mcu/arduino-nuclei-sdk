#include "HardwareSerial.h"
#include <stdio.h>

HardwareSerial Serial(UART_3);
HardwareSerial Serial1(UART_4);

void serialEvent() __attribute__((weak));

void serialEventRun(void)
{
    if(serialEvent && Serial.available()) serialEvent();
}

// Constuctors ///////////////////////////////////////////
HardwareSerial::HardwareSerial(uint32_t uart_x)
{
    // init: set the rx/tx pins
    // init: set the rx/tx buff and their heads and tails
    _serial.uart_periph = uartToPeriph(uart_x);
    _serial.uart_irq = uartToIrq(uart_x);
    _serial.uart_clk = uartToRCU(uart_x);
    _serial.index = uartToIndex(uart_x);
    init(uartToRxPin(uart_x), uartToTxPin(uart_x));
}


void HardwareSerial::init(uint32_t _rx, uint32_t _tx)
{
    _serial.rx_pin = _rx;
    _serial.tx_pin = _tx;
    _serial.rx_buff = _rx_buffer;
    _serial.rx_head = 0;
    _serial.rx_tail = 0;
    _serial.tx_buff = _tx_buffer;
    _serial.tx_head = 0;
    _serial.tx_tail = 0;
}

void HardwareSerial::begin(unsigned long baud, uint8_t config)
{
    uint32_t databits = 0;
    uint32_t stopbits = 0;
    uint32_t parity = 0;

    _baud = baud;
    _config = config;

    switch (config) {
        case SERIAL_8N1:
            databits = USART_WL_8BIT;
            stopbits = USART_STB_1BIT;
            parity = USART_PM_NONE;
            break;
        default:
            break;
    }

    // init the uart, need to delivery "&_serial" to uart.c
    uart_init(&_serial, (uint32_t)baud, databits, parity, stopbits);

    // enable the uart interrupt
    ECLIC_Register_IRQ(_serial.uart_irq, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_LEVEL_TRIGGER, 1, 0, NULL);
    __enable_irq();

    usart_interrupt_enable(_serial.uart_periph, USART_INT_RBNE);
}

void HardwareSerial::end()
{
    flush();

    // deinit the uart
    uart_deinit(&_serial);
    
    // clear received data
    _serial.rx_head = _serial.rx_tail;
}

int HardwareSerial::available(void)
{
    return ((unsigned int)(SERIAL_RX_BUFFER_SIZE + _serial.rx_head - _serial.rx_tail)) % SERIAL_RX_BUFFER_SIZE;
}

int HardwareSerial::peek(void)
{
    if (_serial.rx_head == _serial.rx_tail) {
        return -1;
    } else {
        return _serial.rx_buff[_serial.rx_tail];
    }
}

int HardwareSerial::read()
{
    if (_serial.rx_head == _serial.rx_tail) {
        return -1;
    } else {
        unsigned char c = _serial.rx_buff[_serial.rx_tail];
        _serial.rx_tail = (rx_buffer_index_t)(_serial.rx_tail + 1) % SERIAL_RX_BUFFER_SIZE;
        return c;
    }
}

int HardwareSerial::availableForWrite(void)
{
    tx_buffer_index_t head = _serial.tx_head;
    tx_buffer_index_t tail = _serial.tx_tail;

    if (head >= tail) {
        return SERIAL_TX_BUFFER_SIZE - 1 - head + tail;
    }
    return tail - head - 1;
}

void HardwareSerial::flush()
{
    // If we have never written a byte, no need to flush. This special
    // case is needed since there is no way to force the TXC (transmit
    // complete) bit to 1 during initialization
    if (!_written) {
        return;
    }

    while ((_serial.tx_head != _serial.tx_tail)) {
        // nop, the interrupt handler will free up space for us
    }
    // If we get here, nothing is queued anymore (DRIE is disabled) and
    // the hardware finished tranmission (TXC is set).
}

size_t HardwareSerial::write(uint8_t c)
{
    _written = true;
    
    // if the tx buffer is empty and TBE is set, send char directly and return
    if(_serial.tx_head == _serial.tx_tail && usart_flag_get(_serial.uart_periph, USART_FLAG_TBE)) {

        usart_data_transmit(_serial.uart_periph, c);
        while(!usart_flag_get(_serial.uart_periph, USART_FLAG_TBE));

        return 1;
    }

    tx_buffer_index_t i = (_serial.tx_head + 1) % SERIAL_TX_BUFFER_SIZE;

    // if the tx ring buffer is full, check if the TBE INT is enable
    //// if it's not, enable the TBE INT
    //// if it is, waiting the ISR clear the buffer  
    while(i == _serial.tx_tail) {
        if( !usart_interrupt_flag_get(_serial.uart_periph, USART_INT_FLAG_TBE) ) {
            if( usart_flag_get(_serial.uart_periph, USART_FLAG_TBE) ) {
                usart_interrupt_enable(_serial.uart_periph, USART_FLAG_TBE);
            }
        } else {
            // nop, the interrupt handler will free up space for us
        }
    }


    // other condition, push the char into the tx ring buffer, modify the ptr
    _serial.tx_buff[_serial.tx_head] = c;
    _serial.tx_head = i;
    // enable the TBE INT
    usart_interrupt_enable(_serial.uart_periph, USART_FLAG_TBE);

    return 1;
}



