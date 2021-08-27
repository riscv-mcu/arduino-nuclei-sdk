/*
  TwoWire.cpp - TWI/I2C library for Wiring & Arduino
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
  Modified 2017 by Chuck Todd (ctodd@cableone.net) to correct Unconfigured Slave Mode reboot
  Modified 2020 by Greyson Christoforo (grey@christoforo.net) to implement timeouts
*/

extern "C" {
    #include <stdlib.h>
    #include <string.h>
    #include <inttypes.h>
    #include "utility/twi.h"
}

#include "Wire.h"
#include "Arduino.h"

#define twi_module


// Initialize Class Variables //////////////////////////////////////////////////

uint8_t TwoWire::rxBuffer[BUFFER_LENGTH];
uint8_t TwoWire::rxBufferIndex = 0;
uint8_t TwoWire::rxBufferLength = 0;

uint8_t TwoWire::txAddress = 0;
uint8_t TwoWire::txBuffer[BUFFER_LENGTH];
uint8_t TwoWire::txBufferIndex = 0;
uint8_t TwoWire::txBufferLength = 0;
uint8_t TwoWire::txStatus = 0;

uint8_t TwoWire::transmitting = 0;
void (*TwoWire::user_onRequest)(void);
void (*TwoWire::user_onReceive)(int);

// Constructors ////////////////////////////////////////////////////////////////

TwoWire::TwoWire()
{
}

// Public Methods //////////////////////////////////////////////////////////////

void TwoWire::begin(void)
{
    rxBufferIndex = 0;
    rxBufferLength = 0;

    txBufferIndex = 0;
    txBufferLength = 0;

    twi_init();
    #ifndef twi_module
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_I2C1);

    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

    i2c_clock_config(I2C1, 100000, I2C_DTCY_2);
    i2c_mode_addr_config(I2C1, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0);
    i2c_enable(I2C1);
    i2c_ack_config(I2C1, I2C_ACK_ENABLE);
    
    ECLIC_Register_IRQ(I2C1_EV_IRQn, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_LEVEL_TRIGGER, 2, 0, NULL);
    ECLIC_Register_IRQ(I2C1_ER_IRQn, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_LEVEL_TRIGGER, 3, 0, NULL);
    __enable_irq();
    #endif
}

void TwoWire::begin(uint8_t address)
{
    begin();
    twi_setAddress(address << 1);
}

void TwoWire::begin(int address)
{
    begin((uint8_t)address);
}

void TwoWire::end(void)
{
    twi_disable();
}

void TwoWire::setClock(uint32_t clock)
{
    twi_setFrequency(clock);
}

/***
 * Sets the TWI timeout.
 *
 * This limits the maximum time to wait for the TWI hardware. If more time passes, the bus is assumed
 * to have locked up (e.g. due to noise-induced glitches or faulty slaves) and the transaction is aborted.
 * Optionally, the TWI hardware is also reset, which can be required to allow subsequent transactions to
 * succeed in some cases (in particular when noise has made the TWI hardware think there is a second
 * master that has claimed the bus).
 *
 * When a timeout is triggered, a flag is set that can be queried with `getWireTimeoutFlag()` and is cleared
 * when `clearWireTimeoutFlag()` or `setWireTimeoutUs()` is called.
 *
 * Note that this timeout can also trigger while waiting for clock stretching or waiting for a second master
 * to complete its transaction. So make sure to adapt the timeout to accomodate for those cases if needed.
 * A typical timeout would be 25ms (which is the maximum clock stretching allowed by the SMBus protocol),
 * but (much) shorter values will usually also work.
 *
 * In the future, a timeout will be enabled by default, so if you require the timeout to be disabled, it is
 * recommended you disable it by default using `setWireTimeoutUs(0)`, even though that is currently
 * the default.
 *
 * @param timeout a timeout value in microseconds, if zero then timeout checking is disabled
 * @param reset_with_timeout if true then TWI interface will be automatically reset on timeout
 *                           if false then TWI interface will not be reset on timeout

 */
void TwoWire::setWireTimeout(uint32_t timeout, bool reset_with_timeout)
{
    //todo
    //twi_setTimeoutInMicros(timeout, reset_with_timeout);
}

/***
 * Returns the TWI timeout flag.
 *
 * @return true if timeout has occured since the flag was last cleared.
 */
bool TwoWire::getWireTimeoutFlag(void)
{
    //todo
    //return (twi_manageTimeoutFlag(false));
    return 0;
}

/***
 * Clears the TWI timeout flag.
 */
void TwoWire::clearWireTimeoutFlag(void)
{
    //todo
    //twi_manageTimeoutFlag(true);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop)
{
    // i, internal
    if (isize > 0) {
        // send internal address; this mode allows sending a repeated start to access
        // some devices' internal registers. This function is executed by the hardware
        // TWI module on other processors (for example Due's TWI_IADR and TWI_MMR registers)

        beginTransmission(address);

        // the maximum size of internal address is 3 bytes
        if (isize > 3) {
            isize = 3;
        }

        // write internal register address - most significant byte first
        while (isize-- > 0)
        { write((uint8_t)(iaddress >> (isize * 8))); }
        endTransmission(false);
    }

    // clamp to buffer length
    if (quantity > BUFFER_LENGTH) {
        quantity = BUFFER_LENGTH;
    }
    // perform blocking read into buffer
    uint8_t read_num = twi_readFrom(address, rxBuffer, quantity, sendStop);

    #ifndef twi_module
    /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C1, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C1);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C1, I2C_FLAG_SBSEND));
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C1, address << 1, I2C_RECEIVER);

    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C1, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C1, I2C_FLAG_ADDSEND);

    for(int i=0; i<quantity; i++){
        /* wait until the RBNE bit is set */
        while(!i2c_flag_get(I2C1, I2C_FLAG_RBNE));
        /* read a data from I2C_DATA */
        rxBuffer[i] = i2c_data_receive(I2C1);
    }
    /* send a NACK for the last data byte */
    i2c_ack_config(I2C1, I2C_ACK_DISABLE);

    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C1);
    /* wait until stop condition generate */
    while(I2C_CTL0(I2C1)&0x0200);

    i2c_ack_config(I2C1, I2C_ACK_ENABLE);
    #endif

    // set rx buffer iterator vars
    rxBufferIndex = 0;
    rxBufferLength = quantity;

    return read_num;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop)
{
    return requestFrom((uint8_t)address, (uint8_t)quantity, (uint32_t)0, (uint8_t)0, (uint8_t)sendStop);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity)
{
    return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)true);
}

uint8_t TwoWire::requestFrom(int address, int quantity)
{
    return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)true);
}

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop)
{
    return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)sendStop);
}

void TwoWire::beginTransmission(uint8_t address)
{
    // indicate that we are transmitting
    transmitting = 1;
    // set address of targeted slave
    //txAddress = address;
    txAddress = address << 1;
    // reset tx buffer iterator vars
    txBufferIndex = 0;
    txBufferLength = 0;
}

void TwoWire::beginTransmission(int address)
{
    beginTransmission((uint8_t)address);
}

//
//  Originally, 'endTransmission' was an f(void) function.
//  It has been modified to take one parameter indicating
//  whether or not a STOP should be performed on the bus.
//  Calling endTransmission(false) allows a sketch to
//  perform a repeated start.
//
//  WARNING: Nothing in the library keeps track of whether
//  the bus tenure has been properly ended with a STOP. It
//  is very possible to leave the bus in a hung state if
//  no call to endTransmission(true) is made. Some I2C
//  devices will behave oddly if they do not see a STOP.
//
uint8_t TwoWire::endTransmission(uint8_t sendStop)
{
    // transmit buffer (blocking)
    uint8_t ret = twi_writeTo(txAddress, txBuffer, txBufferLength, 1, sendStop);

    #ifndef twi_module
    /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C1, I2C_FLAG_I2CBSY));

    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C1);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C1, I2C_FLAG_SBSEND));
    
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C1, txAddress, I2C_TRANSMITTER);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C1, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C1, I2C_FLAG_ADDSEND);
    
    for(int i=0; i<txBufferLength; i++){
        /* data transmission */
        i2c_data_transmit(I2C1, txBuffer[i]);
        /* wait until the TBE bit is set */
        while(!i2c_flag_get(I2C1, I2C_FLAG_TBE));
    }
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C1);
    /* wait until stop condition generate */
    while(I2C_CTL0(I2C1)&0x0200);
    #endif

    // reset tx buffer iterator vars
    txBufferIndex = 0;
    txBufferLength = 0;
    // indicate that we are done transmitting
    transmitting = 0;
    return txBufferLength;
}

//  This provides backwards compatibility with the original
//  definition, and expected behaviour, of endTransmission
//
uint8_t TwoWire::endTransmission(void)
{
    return endTransmission(true);
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWire::write(uint8_t data)
{
    if(transmitting){
        // in master transmitter mode
        // don't bother if buffer is full
        if(txBufferLength >= BUFFER_LENGTH){
        setWriteError();
        return 0;
        }
        // put byte in tx buffer
        txBuffer[txBufferIndex] = data;
        ++txBufferIndex;
        // update amount in buffer   
        txBufferLength = txBufferIndex;
    }else{
        // in slave send mode
        // reply to master
        twi_transmit(&data, 1);
    }

    return 1;
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWire::write(const uint8_t* data, size_t quantity)
{
    if(transmitting){
    // in master transmitter mode
        for(size_t i = 0; i < quantity; ++i){
        write(data[i]);
        }
    }else{
    // in slave send mode
        // reply to master
        twi_transmit(data, quantity);
    }

    return quantity;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::available(void)
{
    return rxBufferLength - rxBufferIndex;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::read(void)
{
    int value = -1;

    // get each successive byte on each call
    if(rxBufferIndex < rxBufferLength){
        value = rxBuffer[rxBufferIndex];
        ++rxBufferIndex;
        if(rxBufferIndex == rxBufferLength) {
            rxBufferIndex = 0;
            rxBufferLength = 0;
        }
    }

    return value;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::peek(void)
{
    int value = -1;
    
    if(rxBufferIndex < rxBufferLength){
        value = rxBuffer[rxBufferIndex];
    }

    return value;
}

void TwoWire::flush(void)
{
    // XXX: to be implemented.
}

// sets function called on slave write
void TwoWire::onReceive(void (*function)(int))
{
    user_onReceive = function;
    twi_attachSlaveRxEvent(rxBuffer, &rxBufferIndex, &rxBufferLength, user_onReceive);
}

// sets function called on slave read
void TwoWire::onRequest(void (*function)(void))
{
    user_onRequest = function;
    twi_attachSlaveTxEvent(user_onRequest); // default callback must exist
}

// Preinstantiate Objects //////////////////////////////////////////////////////

TwoWire Wire = TwoWire();