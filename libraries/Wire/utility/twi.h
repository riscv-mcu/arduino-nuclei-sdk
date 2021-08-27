#ifndef twi_h
#define twi_h

#include <inttypes.h>

// PB10 - SCL
// PC11 - SDA

#ifndef TWI_FREQ
#define TWI_FREQ 100000L
#endif

#ifndef TWI_BUFFER_LENGTH
#define TWI_BUFFER_LENGTH 32
#endif

#define TWI_READY 0
#define TWI_MRX   1
#define TWI_MTX   2
#define TWI_SRX   3
#define TWI_STX   4

void twi_init(void);
void twi_disable(void);
void twi_setAddress(uint8_t);
void twi_setFrequency(uint32_t);
uint8_t twi_readFrom(uint8_t, uint8_t*, uint8_t, uint8_t);
uint8_t twi_writeTo(uint8_t, uint8_t*, uint8_t, uint8_t, uint8_t);
uint8_t twi_transmit(const uint8_t*, uint8_t);
void twi_attachSlaveRxEvent(uint8_t*, uint8_t*, uint8_t*, void (*)(int));
void twi_attachSlaveTxEvent(void (*)(void));
void twi_reply(uint8_t);
void twi_stop(void);
void twi_releaseBus(void);
void twi_setTimeoutInMicros(uint32_t, bool);
void twi_handleTimeout(bool);
bool twi_manageTimeoutFlag(bool);

void i2c_writeTo(uint8_t, uint8_t*, uint8_t, uint8_t*);
void I2C1_EV_Handler(void);

#endif
