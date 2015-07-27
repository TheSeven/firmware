#include "global.h"
#include "device/ds1820/ds1820.h"
#include "sys/time.h"
#include "sys/util.h"


DS1820::DS1820(const OneWire::Bus* bus, uint64_t deviceId) : Device(bus, deviceId)
{
    readPowerSupply();
    convertTemperature();
}

int DS1820_OPTIMIZE DS1820::readTemperature()
{
     return scaleRawTemperature(readRawTemperature());
}

int DS1820_OPTIMIZE DS1820::scaleRawTemperature(int data)
{
     return (data * 1000) >> 4;
}

int DS1820_OPTIMIZE DS1820::readRawTemperature()
{
    if (!resolution) setResolution(12);
    uint16_t data;
    convertTemperature();
    readScratchpad((uint8_t*)&data, 2);
    return data;
}

void DS1820_OPTIMIZE DS1820::setResolution(int bits)
{
    resolution = bits;
    uint8_t scratchpad[3] = { 0xff, 0x00, (uint8_t)((MAX(0, MIN(3, bits - 9)) << 5) | 0x1f) };
    writeScratchpad(scratchpad);
}

const uint64_t* DS1820_OPTIMIZE DS1820::getDeviceId()
{
    return Device::getDeviceId();
}

void DS1820_OPTIMIZE DS1820::readScratchpad(uint8_t* data, int len)
{
    select();
    writeByte(0xbe);
    readBytes(data, len);
    sleep();
}

void DS1820_OPTIMIZE DS1820::writeScratchpad(const uint8_t* data)
{
    select();
    writeByte(0x4e);
    writeBytes(data, 3);
    sleep();
}

void DS1820_OPTIMIZE DS1820::writeEEPROM()
{
    select();
    writeByte(0x48);
    if (!externalPower) turnOnPower();
    udelay(10000);
    if (!externalPower) turnOffPower();
    sleep();
}

void DS1820_OPTIMIZE DS1820::readEEPROM()
{
    select();
    writeByte(0xb8);
    int timeout = TIMEOUT_SETUP(100);
    while (!TIMEOUT_EXPIRED(timeout) && !readBit());
    sleep();
}

void DS1820_OPTIMIZE DS1820::readPowerSupply()
{
    select();
    writeByte(0xb4);
    externalPower = readBit();
    sleep();
}

void DS1820_OPTIMIZE DS1820::convertTemperature()
{
    select();
    writeByte(0x44);
    if (externalPower)
    {
        int timeout = TIMEOUT_SETUP((1 << resolution) / 5);
        while (!TIMEOUT_EXPIRED(timeout) && !readBit()) wait(timeout);
    }
    else
    {
        turnOnPower();
        int timeout = TIMEOUT_SETUP((1 << resolution) / 5);
        while (!TIMEOUT_EXPIRED(timeout)) wait(timeout);
        turnOffPower();
    }
    sleep();
}

void __attribute__((weak)) DS1820_OPTIMIZE DS1820::wait(int timeout)
{
}
