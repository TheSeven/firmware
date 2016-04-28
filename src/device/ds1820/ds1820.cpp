#include "global.h"
#include "device/ds1820/ds1820.h"
#include "sys/time.h"
#include "sys/util.h"
#ifdef DS1820_CHECK_CRC
#include "lib/crc8/crc8.h"
#endif


DS1820_OPTIMIZE DS1820::DS1820(const OneWire::Bus* bus, uint64_t deviceId) : Device(bus, deviceId)
{
    readPowerSupply();
}

int DS1820_OPTIMIZE DS1820::readTemperature()
{
     return scaleRawTemperature(readRawTemperature());
}

int DS1820_OPTIMIZE DS1820::scaleRawTemperature(int data)
{
     return (((data << 20) >> 20) * 1000) >> 4;
}

int DS1820_OPTIMIZE DS1820::readRawTemperature()
{
    if (!resolution) setResolution(12);
    convertTemperature();
#ifdef DS1820_CHECK_CRC
    uint8_t data[9];
    int tries = 5;
    while (tries--)
    {
        readScratchpad(data, sizeof(data));
        if (crc8(data, sizeof(data) - 1) == data[sizeof(data) - 1]) break;
    }
    if (tries < 0) return 0x8000;
#else
    uint8_t data[2];
    readScratchpad(data, sizeof(data));
#endif
    return *data | ((data[1] & 0xf) << 8) | ((resolution - 9) << 12) | externalPower << 14;
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
    writeByte(0xbe, false);
    readBytes(data, len, false);
    sleep();
}

void DS1820_OPTIMIZE DS1820::writeScratchpad(const uint8_t* data)
{
    select();
    writeByte(0x4e, false);
    writeBytes(data, 3, false);
    sleep();
}

void DS1820_OPTIMIZE DS1820::writeEEPROM()
{
    select();
#ifdef ONEWIRE_IRQ_LOCKOUT
    bool lockout = externalPower | get_critsec_state();
#endif
    writeByte(0x48, !externalPower);
    if (!externalPower) turnOnPower();
#ifdef ONEWIRE_IRQ_LOCKOUT
    if (!lockout) leave_critical_section();
#endif
    udelay(10000);
    if (!externalPower) turnOffPower();
    sleep();
}

void DS1820_OPTIMIZE DS1820::readEEPROM()
{
    select();
    writeByte(0xb8, false);
    int timeout = TIMEOUT_SETUP(100);
    while (!TIMEOUT_EXPIRED(timeout) && !readBit(false));
    sleep();
}

void DS1820_OPTIMIZE DS1820::readPowerSupply()
{
    select();
    writeByte(0xb4, false);
    externalPower = readBit(false);
    sleep();
}

void DS1820_OPTIMIZE DS1820::convertTemperature()
{
    select();
    if (externalPower)
    {
        writeByte(0x44, false);
        int timeout = TIMEOUT_SETUP((1 << (resolution + 10)) / 5);
        while (!TIMEOUT_EXPIRED(timeout) && !readBit(false)) wait(timeout);
    }
    else
    {
#ifdef ONEWIRE_IRQ_LOCKOUT
        bool lockout = get_critsec_state();
#endif
        writeByte(0x44, true);
        turnOnPower();
#ifdef ONEWIRE_IRQ_LOCKOUT
    if (!lockout) leave_critical_section();
#endif
        int timeout = TIMEOUT_SETUP((1 << (resolution + 10)) / 5);
        while (!TIMEOUT_EXPIRED(timeout)) wait(timeout);
        turnOffPower();
    }
    sleep();
}

void __attribute__((weak)) DS1820_OPTIMIZE DS1820::wait(int timeout)
{
}
