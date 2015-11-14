#pragma once

#include "global.h"
#include "interface/i2c/i2c.h"


#ifndef SI7021_OPTIMIZE
#define SI7021_OPTIMIZE
#endif


class __attribute__((packed,aligned(4))) SI7021 final
{
public:
    SI7021(I2C::Bus* bus);
    constexpr SI7021() : bus(NULL), statusReg{0} {}
    int readTemperature();
    uint16_t readRawTemperature();
    int readLastTemperature();
    uint16_t readRawLastTemperature();
    int readHumidity();
    uint16_t readRawHumidity();
    void setResolution(int mode);
    void setHeaterPower(int level);
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
            uint8_t : 1;
            uint8_t enableHeater : 1;
            uint8_t : 3;
            uint8_t batteryStatus : 1;
            uint8_t resolutionHigh : 1;
        } b;
    } statusReg;
};
