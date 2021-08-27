#include <math.h>
#include <stdlib.h>
#include <inttypes.h>
#include "Arduino.h" // for digitalWrite and micros

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#include "pins_arduino.h"
#include "twi.h"

typedef void (*voidFuncPtr)(void);

static void req_nothing(void)
{
    // do nothing
}

static void rec_nothing(int n)
{
    // do nothing
}

static volatile void (*reqFunc)(void) = req_nothing;
static volatile void (*recFunc)(int) = rec_nothing;

static uint8_t* rx_buffer_addr = NULL;
static uint8_t* rx_buffer_index = NULL;
static uint8_t* rx_buffer_length = NULL;

static volatile uint8_t twi_state;
static volatile uint8_t twi_slarw;
static volatile uint8_t twi_sendStop;           // should the transaction end with a stop
static volatile uint8_t twi_inRepStart;         // in the middle of a repeated start

// twi_timeout_us > 0 prevents the code from getting stuck in various while loops here
// if twi_timeout_us == 0 then timeout checking is disabled (the previous Wire lib behavior)
// at some point in the future, the default twi_timeout_us value could become 25000
// and twi_do_reset_on_timeout could become true
// to conform to the SMBus standard
// http://smbus.org/specs/SMBus_3_1_20180319.pdf
static volatile uint32_t twi_timeout_us = 0ul;
static volatile bool twi_timed_out_flag = false;  // a timeout has been seen
static volatile bool twi_do_reset_on_timeout = false;  // reset the TWI registers on timeout

static void (*twi_onSlaveTransmit)(void);
static void (*twi_onSlaveReceive)(uint8_t*, int);

static uint8_t twi_masterBuffer[TWI_BUFFER_LENGTH];
static volatile uint8_t twi_masterBufferIndex;
static volatile uint8_t twi_masterBufferLength;

static uint8_t twi_txBuffer[TWI_BUFFER_LENGTH];
static volatile uint8_t twi_txBufferIndex;
static volatile uint8_t twi_txBufferLength;

static uint8_t twi_rxBuffer[TWI_BUFFER_LENGTH];
static volatile uint8_t twi_rxBufferIndex;

static volatile uint8_t twi_error;

/*
 * Function twi_init
 * Desc     readys twi pins and sets twi bitrate
 * Input    none
 * Output   none
 */
void twi_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_I2C1);

    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

    i2c_clock_config(I2C1, 100000, I2C_DTCY_2);
    i2c_mode_addr_config(I2C1, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0);
    i2c_enable(I2C1);
    i2c_ack_config(I2C1, I2C_ACK_ENABLE);

    ECLIC_Register_IRQ(I2C1_EV_IRQn, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_LEVEL_TRIGGER, 1, 0, NULL);
    //ECLIC_Register_IRQ(I2C1_ER_IRQn, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_LEVEL_TRIGGER, 1, 0, NULL);
    __enable_irq();
}

/*
 * Function twi_disable
 * Desc     disables twi pins
 * Input    none
 * Output   none
 */
void twi_disable(void)
{
    // disable twi module, acks, and twi interrupt
    i2c_ack_config(I2C1, I2C_ACK_DISABLE);
    i2c_disable(I2C1);
    i2c_deinit(I2C1);

    // deactivate internal pullups for twi.
    digitalWrite(SDA, 0);
    digitalWrite(SCL, 0);
}

/*
 * Function twi_slaveInit
 * Desc     sets slave address and enables interrupt
 * Input    none
 * Output   none
 */
void twi_setAddress(uint8_t address)
{
    i2c_mode_addr_config(I2C1, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, address);
}

/*
 * Function twi_setClock
 * Desc     sets twi bit rate
 * Input    Clock Frequency
 * Output   none
 */
void twi_setFrequency(uint32_t frequency)
{
    i2c_clock_config(I2C1, frequency, I2C_DTCY_2);
}

/*
 * Function twi_readFrom
 * Desc     attempts to become twi bus master and read a
 *          series of bytes from a device on the bus
 * Input    address: 7bit i2c device address
 *          data: pointer to byte array
 *          length: number of bytes to read into array
 *          sendStop: Boolean indicating whether to send a stop at the end
 * Output   number of bytes read
 */
uint8_t twi_readFrom(uint8_t address, uint8_t* data, uint8_t length, uint8_t sendStop)
{
    /* wait until I2C bus is idle */
    while (i2c_flag_get(I2C1, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C1);
    /* wait until SBSEND bit is set */
    while (!i2c_flag_get(I2C1, I2C_FLAG_SBSEND));
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C1, address << 1, I2C_RECEIVER);

    /* wait until ADDSEND bit is set */
    while (!i2c_flag_get(I2C1, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C1, I2C_FLAG_ADDSEND);

    uint8_t count = 0;
    for (uint8_t i = 0; i < length; i++) {
        /* wait until the RBNE bit is set */
        while (!i2c_flag_get(I2C1, I2C_FLAG_RBNE));
        /* read a data from I2C_DATA */
        data[i] = i2c_data_receive(I2C1);
        count++;
    }
    /* send a NACK for the last data byte */
    i2c_ack_config(I2C1, I2C_ACK_DISABLE);

    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C1);
    /* wait until stop condition generate */
    while (I2C_CTL0(I2C1) & 0x0200);

    i2c_ack_config(I2C1, I2C_ACK_ENABLE);

    return count;
}

/*
 * Function twi_writeTo
 * Desc     attempts to become twi bus master and write a
 *          series of bytes to a device on the bus
 * Input    address: 7bit i2c device address
 *          data: pointer to byte array
 *          length: number of bytes in array
 *          wait: boolean indicating to wait for write or not
 *          sendStop: boolean indicating whether or not to send a stop at the end
 * Output   0 .. success
 *          1 .. length to long for buffer
 *          2 .. address send, NACK received
 *          3 .. data send, NACK received
 *          4 .. other twi error (lost bus arbitration, bus error, ..)
 *          5 .. timeout
 */
uint8_t twi_writeTo(uint8_t address, uint8_t* data, uint8_t length, uint8_t wait, uint8_t sendStop)
{
    /* wait until I2C bus is idle */
    while (i2c_flag_get(I2C1, I2C_FLAG_I2CBSY));

    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C1);
    /* wait until SBSEND bit is set */
    while (!i2c_flag_get(I2C1, I2C_FLAG_SBSEND));

    /* send slave address to I2C bus */
    i2c_master_addressing(I2C1, address, I2C_TRANSMITTER);
    /* wait until ADDSEND bit is set */
    while (!i2c_flag_get(I2C1, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C1, I2C_FLAG_ADDSEND);

    uint8_t count = 0;
    for (uint8_t i = 0; i < length; i++) {
        /* data transmission */
        i2c_data_transmit(I2C1, data[i]);
        /* wait until the TBE bit is set */
        while (!i2c_flag_get(I2C1, I2C_FLAG_TBE));
        count++;
    }
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C1);
    /* wait until stop condition generate */
    while (I2C_CTL0(I2C1) & 0x0200);

    return count;
}

/*
 * Function twi_transmit
 * Desc     fills slave tx buffer with data
 *          must be called in slave tx event callback
 * Input    data: pointer to byte array
 *          length: number of bytes in array
 * Output   1 length too long for buffer
 *          2 not slave transmitter
 *          0 ok
 */
uint8_t twi_transmit(const uint8_t* data, uint8_t length)
{
    /* wait until ADDSEND bit is set */
    //while(!i2c_flag_get(I2C1, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    //i2c_flag_clear(I2C1, I2C_FLAG_ADDSEND);
    /* wait until the transmission data register is empty */
    while (!i2c_flag_get(I2C1, I2C_FLAG_TBE));

    for (uint8_t i = 0; i < length; i++) {
        /* send a data byte */
        i2c_data_transmit(I2C1, data[i]);
        /* wait until the transmission data register is empty */
        while (!i2c_flag_get(I2C1, I2C_FLAG_TBE));
    }
    /* the master doesn't acknowledge for the last byte */
    while (!i2c_flag_get(I2C1, I2C_FLAG_AERR));
    /* clear the bit of AERR */
    i2c_flag_clear(I2C1, I2C_FLAG_AERR);
}

/*
 * Function twi_attachSlaveRxEvent
 * Desc     sets function called before a slave read operation
 * Input    function: callback function to use
 * Output   none
 */
//void twi_attachSlaveRxEvent(void (*function)(uint8_t*, int))
void twi_attachSlaveRxEvent(uint8_t* addr, uint8_t* index, uint8_t* length, void (*function)(int))
{
    rx_buffer_addr = addr;
    rx_buffer_index = index;
    rx_buffer_length = length;
    recFunc = function;
    i2c_interrupt_enable(I2C1, I2C_INT_EV);
    i2c_interrupt_enable(I2C1, I2C_INT_BUF);
}

/*
 * Function twi_attachSlaveTxEvent
 * Desc     sets function called before a slave write operation
 * Input    function: callback function to use
 * Output   none
 */
void twi_attachSlaveTxEvent(void (*function)(void))
{
    reqFunc = function;
    i2c_interrupt_enable(I2C1, I2C_INT_EV);
}

/*
 * Function twi_reply
 * Desc     sends byte or readys receive line
 * Input    ack: byte indicating to ack or to nack
 * Output   none
 */
void twi_reply(uint8_t ack)
{

}

/*
 * Function twi_stop
 * Desc     relinquishes bus master status
 * Input    none
 * Output   none
 */
void twi_stop(void)
{

}

/*
 * Function twi_releaseBus
 * Desc     releases bus control
 * Input    none
 * Output   none
 */
void twi_releaseBus(void)
{

}

/*
 * Function twi_setTimeoutInMicros
 * Desc     set a timeout for while loops that twi might get stuck in
 * Input    timeout value in microseconds (0 means never time out)
 * Input    reset_with_timeout: true causes timeout events to reset twi
 * Output   none
 */
void twi_setTimeoutInMicros(uint32_t timeout, bool reset_with_timeout)
{

}

/*
 * Function twi_handleTimeout
 * Desc     this gets called whenever a while loop here has lasted longer than
 *          twi_timeout_us microseconds. always sets twi_timed_out_flag
 * Input    reset: true causes this function to reset the twi hardware interface
 * Output   none
 */
void twi_handleTimeout(bool reset)
{

}

/*
 * Function twi_manageTimeoutFlag
 * Desc     returns true if twi has seen a timeout
 *          optionally clears the timeout flag
 * Input    clear_flag: true if we should reset the hardware
 * Output   the value of twi_timed_out_flag when the function was called
 */
bool twi_manageTimeoutFlag(bool clear_flag)
{
    return clear_flag;
}

void I2C1_EV_IRQHandler(void)
{
    if (i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_ADDSEND)) {
        /* clear the ADDSEND bit */
        i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_ADDSEND);
    } else if (i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_RBNE)) {
        rx_buffer_addr[(*rx_buffer_length)++] = i2c_data_receive(I2C1);
        //*rx_buffer_length = ((*rx_buffer_length) + 1) % 32;
    } else if (i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_STPDET)) {
        /* clear the STPDET bit */
        i2c_enable(I2C1);
        recFunc(1);
    } else {
        /* send a data byte */
        reqFunc();
    }
}