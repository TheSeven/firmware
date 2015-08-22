#include "global.h"
#include "device/htu21d/htu21d.h"
#include "sys/util.h"


HTU21D_OPTIMIZE HTU21D::HTU21D(I2C::Bus* bus) : bus(bus)
{
    writeStatusReg();
}

int HTU21D_OPTIMIZE HTU21D::readTemperature()
{
     return scaleRawTemperature(readRawTemperature());
}

int HTU21D_OPTIMIZE HTU21D::scaleRawTemperature(uint16_t data)
{
     return ((175720 * data) >> 14) - 46850;
}

uint16_t HTU21D_OPTIMIZE HTU21D::readRawTemperature()
{
    uint16_t data;
    for (int i = 0; i < 3; i++)
        if (bus->readRegs(0x40, 0xe3, &data, sizeof(data)) == I2C::RESULT_OK)
            break;
    return swap16(data) >> 2;
}

int HTU21D_OPTIMIZE HTU21D::readHumidity()
{
     return scaleRawTemperature(readRawTemperature());
}

int HTU21D_OPTIMIZE HTU21D::scaleRawHumidity(uint16_t data)
{
     return ((125000 * data) >> 14) - 6000;
}

uint16_t HTU21D_OPTIMIZE HTU21D::readRawHumidity()
{
    uint16_t data;
    for (int i = 0; i < 3; i++)
        if (bus->readRegs(0x40, 0xe5, &data, sizeof(data)) == I2C::RESULT_OK)
            break;
    return swap16(data) >> 2;
}

void HTU21D_OPTIMIZE HTU21D::setResolution(int mode)
{
    statusReg.b.resolutionHigh = (mode >> 1) & 1;
    statusReg.b.resolutionLow = mode & 1;
    writeStatusReg();
}

void HTU21D_OPTIMIZE HTU21D::enableHeater(bool on)
{
    statusReg.b.enableHeater = on;
    writeStatusReg();
}

bool HTU21D_OPTIMIZE HTU21D::getLowBattery()
{
    readStatusReg();
    return statusReg.b.batteryStatus;
}

void HTU21D_OPTIMIZE HTU21D::readStatusReg()
{
    for (int i = 0; i < 3; i++)
        if (bus->readRegs(0x40, 0xe6, &statusReg, sizeof(statusReg)) == I2C::RESULT_OK)
            break;
}

void HTU21D_OPTIMIZE HTU21D::writeStatusReg()
{
    for (int i = 0; i < 3; i++)
        if (bus->writeRegs(0x40, 0xe7, &statusReg, sizeof(statusReg)) == I2C::RESULT_OK)
            break;
}
