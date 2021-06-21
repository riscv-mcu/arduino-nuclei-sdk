
#include "SPI.h"
#include <gd32vf103.h>


SPIClass::SPIClass()
    : _dev(SPI0)
    , _dev_clk(RCU_SPI0)
    , _mosi_bank(GPIOA)
    , _miso_bank(GPIOA)
    , _sclk_bank(GPIOA)
    , _ssel_bank(GPIOB)
    , _mosi_bit(GPIO_PIN_7)
    , _miso_bit(GPIO_PIN_6)
    , _sclk_bit(GPIO_PIN_5)
    , _ssel_bit(GPIO_PIN_9)
    , _mosi_bank_clk(RCU_GPIOA)
    , _miso_bank_clk(RCU_GPIOA)
    , _sclk_bank_clk(RCU_GPIOA)
    , _ssel_bank_clk(RCU_GPIOB) 
    , _ssel_hard(1) {
    //
}

void SPIClass::begin() {
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    
    /* configure SPI0 GPIO : SCK/PA5, MISO/PA6, MOSI/PA7, NSS/PB9*/
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    //gpio_bit_set(GPIOB, GPIO_PIN_9);
    
    rcu_periph_clock_enable(RCU_SPI0);
    spi_i2s_deinit(SPI0);
}

void SPIClass::end() {
    spi_i2s_deinit(SPI0);
    rcu_periph_clock_disable(RCU_SPI0);
}

void SPIClass::beginTransaction(SPISettings settings) {
    if (_dev == 0) {
        return;
    }
    if (settings._freq == 0) {
        return;
    }
    if (_dev == SPI0 && settings._freq > rcu_clock_freq_get(CK_APB2)) {
        return;
    }
    else if (settings._freq > rcu_clock_freq_get(CK_APB1)) {
        return;
    }
    _dataMode = settings._dataMode;
    _bitOrder = settings._bitOrder;
    _freq = settings._freq;
    beginTransaction();
}

void SPIClass::endTransaction() {
    if (_dev == 0) {
        return;
    }
    spi_disable(_dev);
}

void SPIClass::transfer(uint8_t* data, uint32_t size, uint32_t timeout) {
    if (_dev == 0 || data == nullptr) {
        return;
    }
    uint64_t startT = millis();

    if (_ssel_hard == 0 && _ssel_bank != 0) {
        gpio_bit_reset(_ssel_bank, _ssel_bit);
    }

    for (size_t i = 0; i < size; i++) {
        while (!spi_i2s_flag_get(_dev, SPI_FLAG_TBE)) {
            if (millis() > startT + timeout) {
                if (_ssel_hard == 0 && _ssel_bank != 0) {
                    gpio_bit_set(_ssel_bank, _ssel_bit);
                }
                return;
            }
        }
        spi_i2s_data_transmit(_dev, *data);
        data++;
    }

    if (_ssel_hard == 0 && _ssel_bank != 0) {
        gpio_bit_set(_ssel_bank, _ssel_bit);
    }
}

void SPIClass::transfer(
    uint8_t* txdata, uint8_t* rxdata, uint32_t size, uint32_t timeout) {
    if (_dev == 0 || txdata == nullptr || rxdata == nullptr) {
        return;
    }
    uint64_t startT = millis();

    if (_ssel_hard == 0 && _ssel_bank != 0) {
        gpio_bit_reset(_ssel_bank, _ssel_bit);
    }

    for (size_t i = 0; i < size; i++) {
        while (!spi_i2s_flag_get(_dev, SPI_FLAG_TBE)) {
            if (millis() > startT + timeout) {
                if (_ssel_hard == 0 && _ssel_bank != 0) {
                    gpio_bit_set(_ssel_bank, _ssel_bit);
                }
                return;
            }
        }
        spi_i2s_data_transmit(_dev, *txdata);
        txdata++;
        while (!spi_i2s_flag_get(_dev, SPI_FLAG_RBNE)) {
            if (millis() > startT + timeout) {
                if (_ssel_hard == 0 && _ssel_bank != 0) {
                    gpio_bit_set(_ssel_bank, _ssel_bit);
                }
                return;
            }
        }
        *rxdata = spi_i2s_data_receive(_dev);
        rxdata++;
    }

    if (_ssel_hard == 0 && _ssel_bank != 0) {
        gpio_bit_set(_ssel_bank, _ssel_bit);
    }
}

void SPIClass::setBitOrder(uint8_t bitOrder) {
    _bitOrder = bitOrder;
    beginTransaction();
}

void SPIClass::setDataMode(uint8_t dataMode) {
    _dataMode = dataMode;
    beginTransaction();
}

void SPIClass::setFrequency(uint32_t freq) {
    if (_dev == 0) {
        return;
    }
    if (freq == 0) {
        return;
    }
    if (_dev == SPI0 && freq > rcu_clock_freq_get(CK_APB2)) {
        return;
    }
    else if (freq > rcu_clock_freq_get(CK_APB1)) {
        return;
    }
    _freq = freq;
    beginTransaction();
}

void SPIClass::beginTransaction() {
    if (_dev == 0) {
        return;
    }
    spi_parameter_struct param;
    switch (_dataMode) {
    case SPI_MODE0:
        param.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
        break;
    case SPI_MODE1:
        param.clock_polarity_phase = SPI_CK_PL_HIGH_PH_1EDGE;
        break;
    case SPI_MODE2:
        param.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
        break;
    case SPI_MODE3:
        param.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
        break;
    default:
        return;
    }
    param.device_mode = SPI_MASTER;
    param.endian = _bitOrder == MSBFIRST ? SPI_ENDIAN_MSB : SPI_ENDIAN_LSB;
    param.frame_size = SPI_FRAMESIZE_8BIT;
    param.nss = _ssel_hard ? SPI_NSS_HARD : SPI_NSS_SOFT;
    uint32_t prescale = 0;
    if (_dev == SPI0) {
        prescale = rcu_clock_freq_get(CK_APB2) / _freq - 1;
    }
    else {
        prescale = rcu_clock_freq_get(CK_APB1) / _freq - 1;
    }
    param.prescale = prescale;
    param.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spi_init(_dev, &param);
    spi_enable(_dev);
}