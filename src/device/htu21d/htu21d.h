#pragma once

#include "global.h"
#include "interface/i2c/i2c.h"


#ifndef HTU21D_OPTIMIZE
#define HTU21D_OPTIMIZE
#endif


class __attribute__((packed,aligned(4))) HTU21D final
{
public:
    HTU21D(I2C::Bus* bus);
    constexpr HTU21D() : bus(NULL), statusReg{0} {}
    int readTemperature();
    uint16_t readRawTemperature();
    int readHumidity();
    uint16_t readRawHumidity();
    void setResolution(int mode);
    void enableHeater(bool on);
    bool getLowBattery();

    static int scaleRawTemperature(uint16_t data);
    static int scaleRawHumidity(uint16_t data);

private:
    void readStatusReg();
    void writeStatusReg();

    I2C::Bus* bus;
    union __attribute__((packed))
    {
        uint8_t u8;
        struct __attribute__((packed))
        {
            uint8_t resolutionLow : 1;
            uint8_t disableOTPReload : 1;
            uint8_t enableHeater : 1;
            uint8_t : 3;
            uint8_t batteryStatus : 1;
            uint8_t resolutionHigh : 1;
        } b;
    } statusReg;
};
