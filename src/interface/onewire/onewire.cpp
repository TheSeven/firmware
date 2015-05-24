#include "global.h"
#include "interface/onewire/onewire.h"
#include "sys/util.h"
#include "sys/time.h"


OneWire::Bus::Bus(GPIO::Pin pin) : pin(pin)
{
    GPIO::configure(pin, GPIO::MODE_OUTPUT, true, GPIO::TYPE_OPENDRAIN, GPIO::PULL_NONE);
    resetDiscovery();
}

void ONEWIRE_OPTIMIZE OneWire::Bus::sleep() const
{
    GPIO::setPull(pin, GPIO::PULL_NONE);
}

bool __attribute__((weak)) ONEWIRE_OPTIMIZE OneWire::Bus::reset() const
{
    GPIO::setPull(pin, GPIO::PULL_UP);
    int time = TIMEOUT_SETUP(1000);
    while (!GPIO::getLevel(pin)) if (TIMEOUT_EXPIRED(time)) return false;
    time = delay(read_usec_timer() + 100);
    GPIO::setLevel(pin, false);
    time = delay(time + 480);
    GPIO::setLevel(pin, true);
    time = delay(time + 70);
    bool result = !GPIO::getLevel(pin);
    delay(time + 410);
    return result;
}

void ONEWIRE_OPTIMIZE OneWire::Bus::select(uint64_t deviceId) const
{
    writeByte(0x55);
    writeBytes((uint8_t*)&deviceId, 8);
}

void ONEWIRE_OPTIMIZE OneWire::Bus::selectAll() const
{
    writeByte(0xcc);
}

void __attribute__((weak)) ONEWIRE_OPTIMIZE OneWire::Bus::writeBit(bool bit) const
{
    int time = read_usec_timer();
    GPIO::setLevel(pin, false);
    time = delay(time + (bit ? 5 : 60));
    GPIO::setLevel(pin, true);
    delay(time + (bit ? 55 : 5));
}

void ONEWIRE_OPTIMIZE OneWire::Bus::writeByte(uint8_t byte) const
{
    for (int i = 0; i < 8; i++)
    {
        writeBit(byte & 1);
        byte >>= 1;
    }
}

void ONEWIRE_OPTIMIZE OneWire::Bus::writeBytes(const uint8_t* data, int len) const
{
    while (len--) writeByte(*data++);
}

bool __attribute__((weak)) ONEWIRE_OPTIMIZE OneWire::Bus::readBit() const
{
    int time = read_usec_timer();
    GPIO::setLevel(pin, false);
    time = delay(time + 2);
    GPIO::setLevel(pin, true);
    time = delay(time + 12);
    bool result = GPIO::getLevel(pin);
    delay(time + 50);
    return result;
}

uint8_t ONEWIRE_OPTIMIZE OneWire::Bus::readByte() const
{
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++)
    {
        byte >>= 1;
        if (readBit()) byte |= 0x80;
    }
    return byte;
}

void ONEWIRE_OPTIMIZE OneWire::Bus::readBytes(uint8_t* data, int len) const
{
    while (len--) *data++ = readByte();
}

void ONEWIRE_OPTIMIZE OneWire::Bus::turnOnPower() const
{
    GPIO::setType(pin, GPIO::TYPE_PUSHPULL);
}

void ONEWIRE_OPTIMIZE OneWire::Bus::turnOffPower() const
{
    GPIO::setType(pin, GPIO::TYPE_OPENDRAIN);
}

void ONEWIRE_OPTIMIZE OneWire::Bus::resetDiscovery()
{
    lastDiscrepancy = 0;
    isLastDevice = false;
    scanDeviceId = 0;
}

uint64_t* ONEWIRE_OPTIMIZE OneWire::Bus::discoverDevice()
{
    if (isLastDevice) return NULL;
    if (!reset()) return NULL;
    writeByte(0xf0);
    bool direction;
    int prevDiscrepancy = 0;
    int bitNumber;
    for (bitNumber = 0; bitNumber < 64; bitNumber++)
    {
        bool bitValue = readBit();
        bool bitComplement = readBit();
        if (bitValue && bitComplement) break;
        if (bitValue != bitComplement) direction = bitValue;
        else
        {
            if (bitNumber < lastDiscrepancy) direction = (scanDeviceId >> bitNumber) & 1;
            else direction = bitNumber == lastDiscrepancy;
            if (!direction) prevDiscrepancy = bitNumber;
        }
        scanDeviceId = (scanDeviceId & ~(1ull << bitNumber)) | (((uint64_t)direction) << bitNumber);
        writeBit(direction);
    }
    if (bitNumber != 64) return NULL;
    lastDiscrepancy = prevDiscrepancy;
    if (!lastDiscrepancy) isLastDevice = true;
    return &scanDeviceId;
}

uint8_t ONEWIRE_OPTIMIZE OneWire::Bus::crc8(const uint8_t* data, int len)
{
    uint8_t crc = 0;
    while (len--)
    {
        uint8_t byte = *data++;
        for (int i = 0; i < 8; i++)
        {
            crc = (crc >> 1) ^ (((crc ^ byte) & 1) ? 0x8c : 0);
            byte >>= 1;
        }
    }
    return crc;
}

int __attribute__((optimize("-Os"))) OneWire::Bus::delay(int time)
{
    while (true)
    {
        int now = read_usec_timer();
        if (TIME_AFTER(now, time)) return now;
    }
}

void ONEWIRE_OPTIMIZE OneWire::Device::select() const
{
    bus->reset();
    bus->select(deviceId);
}

void ONEWIRE_OPTIMIZE OneWire::Device::sleep() const
{
    bus->sleep();
}

void ONEWIRE_OPTIMIZE OneWire::Device::writeBit(bool bit) const
{
    bus->writeBit(bit);
}

void ONEWIRE_OPTIMIZE OneWire::Device::writeByte(uint8_t byte) const
{
    bus->writeByte(byte);
}

void ONEWIRE_OPTIMIZE OneWire::Device::writeBytes(const uint8_t* data, int len) const
{
    bus->writeBytes(data, len);
}

bool ONEWIRE_OPTIMIZE OneWire::Device::readBit() const
{
    return bus->readBit();
}

uint8_t ONEWIRE_OPTIMIZE OneWire::Device::readByte() const
{
    return bus->readByte();
}

void ONEWIRE_OPTIMIZE OneWire::Device::readBytes(uint8_t* data, int len) const
{
    return bus->readBytes(data, len);
}

void ONEWIRE_OPTIMIZE OneWire::Device::turnOnPower() const
{
    bus->turnOnPower();
}

void ONEWIRE_OPTIMIZE OneWire::Device::turnOffPower() const
{
    bus->turnOffPower();
}

const uint64_t* ONEWIRE_OPTIMIZE OneWire::Device::getDeviceId() const
{
    return &deviceId;
}
