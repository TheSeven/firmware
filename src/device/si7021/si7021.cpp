#include "global.h"
#include "device/si7021/si7021.h"
#include "sys/util.h"


SI7021_OPTIMIZE SI7021::SI7021(I2C::Bus* bus) : bus(bus)
{
    writeStatusReg();
}

int SI7021_OPTIMIZE SI7021::readTemperature()
{
     return scaleRawTemperature(readRawTemperature());
}

int SI7021_OPTIMIZE SI7021::readLastTemperature()
{
     return scaleRawTemperature(readRawLastTemperature());
}

int SI7021_OPTIMIZE SI7021::scaleRawTemperature(uint16_t data)
{
     return ((175720 * data) >> 14) - 46850;
}

uint16_t SI7021_OPTIMIZE SI7021::readRawTemperature()
{
    uint16_t data;
    for (int i = 0; i < 3; i++)
        if (bus->readRegs(0x40, 0xe3, &data, sizeof(data)) == I2C::RESULT_OK)
            break;
    return swap16(data) >> 2;
}

uint16_t SI7021_OPTIMIZE SI7021::readRawLastTemperature()
{
    uint16_t data;
    for (int i = 0; i < 3; i++)
        if (bus->readRegs(0x40, 0xe0, &data, sizeof(data)) == I2C::RESULT_OK)
            break;
    return swap16(data) >> 2;
}

int SI7021_OPTIMIZE SI7021::readHumidity()
{
     return scaleRawTemperature(readRawTemperature());
}

int SI7021_OPTIMIZE SI7021::scaleRawHumidity(uint16_t data)
{
     return ((125000 * data) >> 14) - 6000;
}

uint16_t SI7021_OPTIMIZE SI7021::readRawHumidity()
{
    uint16_t data;
    for (int i = 0; i < 3; i++)
        if (bus->readRegs(0x40, 0xe5, &data, sizeof(data)) == I2C::RESULT_OK)
            break;
    return swap16(data) >> 2;
}

void SI7021_OPTIMIZE SI7021::setResolution(int mode)
{
    statusReg.b.resolutionHigh = (mode >> 1) & 1;
    statusReg.b.resolutionLow = mode & 1;
    writeStatusReg();
}

void SI7021_OPTIMIZE SI7021::setHeaterPower(int level)
{
    for (int i = 0; i < 3; i++)
        if (bus->writeRegs(0x40, 0x51, &level, 1) == I2C::RESULT_OK)
            break;
}

void SI7021_OPTIMIZE SI7021::enableHeater(bool on)
{
    statusReg.b.enableHeater = on;
    writeStatusReg();
}

bool SI7021_OPTIMIZE SI7021::getLowBattery()
{
    readStatusReg();
    return statusReg.b.batteryStatus;
}

void SI7021_OPTIMIZE SI7021::readStatusReg()
{
    for (int i = 0; i < 3; i++)
        if (bus->readRegs(0x40, 0xe6, &statusReg, sizeof(statusReg)) == I2C::RESULT_OK)
            break;
}

void SI7021_OPTIMIZE SI7021::writeStatusReg()
{
    for (int i = 0; i < 3; i++)
        if (bus->writeRegs(0x40, 0xe7, &statusReg, sizeof(statusReg)) == I2C::RESULT_OK)
            break;
}
