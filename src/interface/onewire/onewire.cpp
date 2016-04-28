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
    delay(read_usec_timer() + 100);
#ifdef ONEWIRE_IRQ_LOCKOUT
    bool lockout = get_critsec_state();
    enter_critical_section();
#endif
    time = read_usec_timer();
    GPIO::setLevel(pin, false);
    time = delay(time + 480);
    GPIO::setLevel(pin, true);
    time = delay(time + 70);
    bool result = !GPIO::getLevel(pin);
#ifdef ONEWIRE_IRQ_LOCKOUT
    if (!lockout) leave_critical_section();
#endif
    delay(time + 410);
    return result;
}

void ONEWIRE_OPTIMIZE OneWire::Bus::select(uint64_t deviceId) const
{
    writeByte(0x55, false);
    writeBytes((uint8_t*)&deviceId, 8, false);
}

void ONEWIRE_OPTIMIZE OneWire::Bus::selectAll() const
{
    writeByte(0xcc, false);
}

void __attribute__((weak)) ONEWIRE_OPTIMIZE OneWire::Bus::writeBit(bool bit, bool keepLocked) const
{
#ifdef ONEWIRE_IRQ_LOCKOUT
    bool lockout = keepLocked | get_critsec_state();
    enter_critical_section();
#endif
    int time = read_usec_timer();
    GPIO::setLevel(pin, false);
    time = delay(time + (bit ? 5 : 60));
    GPIO::setLevel(pin, true);
#ifdef ONEWIRE_IRQ_LOCKOUT
    if (!lockout) leave_critical_section();
#endif
    delay(time + (bit ? 55 : 5));
}

void ONEWIRE_OPTIMIZE OneWire::Bus::writeByte(uint8_t byte, bool keepLocked) const
{
    for (int i = 0; i < 8; i++)
    {
        writeBit(byte & 1, keepLocked && i == 7);
        byte >>= 1;
    }
}

void ONEWIRE_OPTIMIZE OneWire::Bus::writeBytes(const uint8_t* data, int len, bool keepLocked) const
{
    while (len--) writeByte(*data++, keepLocked && !len);
}

bool __attribute__((weak)) ONEWIRE_OPTIMIZE OneWire::Bus::readBit(bool keepLocked) const
{
#ifdef ONEWIRE_IRQ_LOCKOUT
    bool lockout = keepLocked | get_critsec_state();
    enter_critical_section();
#endif
    int time = read_usec_timer();
    GPIO::setLevel(pin, false);
    time = delay(time + 2);
    GPIO::setLevel(pin, true);
    time = delay(time + 12);
    bool result = GPIO::getLevel(pin);
#ifdef ONEWIRE_IRQ_LOCKOUT
    if (!lockout) leave_critical_section();
#endif
    delay(time + 50);
    return result;
}

uint8_t ONEWIRE_OPTIMIZE OneWire::Bus::readByte(bool keepLocked) const
{
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++)
    {
        byte >>= 1;
        if (readBit(keepLocked && i == 7)) byte |= 0x80;
    }
    return byte;
}

void ONEWIRE_OPTIMIZE OneWire::Bus::readBytes(uint8_t* data, int len, bool keepLocked) const
{
    while (len--) *data++ = readByte(keepLocked && !len);
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
    writeByte(0xf0, false);
    bool direction;
    int prevDiscrepancy = 0;
    int bitNumber;
    for (bitNumber = 0; bitNumber < 64; bitNumber++)
    {
        bool bitValue = readBit(false);
        bool bitComplement = readBit(false);
        if (bitValue && bitComplement) break;
        if (bitValue != bitComplement) direction = bitValue;
        else
        {
            if (bitNumber < lastDiscrepancy) direction = (scanDeviceId >> bitNumber) & 1;
            else direction = bitNumber == lastDiscrepancy;
            if (!direction) prevDiscrepancy = bitNumber;
        }
        scanDeviceId = (scanDeviceId & ~(1ull << bitNumber)) | (((uint64_t)direction) << bitNumber);
        writeBit(direction, false);
    }
    if (bitNumber != 64) return NULL;
    lastDiscrepancy = prevDiscrepancy;
    if (!lastDiscrepancy) isLastDevice = true;
    return &scanDeviceId;
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

void ONEWIRE_OPTIMIZE OneWire::Device::writeBit(bool bit, bool keepLocked) const
{
    bus->writeBit(bit, keepLocked);
}

void ONEWIRE_OPTIMIZE OneWire::Device::writeByte(uint8_t byte, bool keepLocked) const
{
    bus->writeByte(byte, keepLocked);
}

void ONEWIRE_OPTIMIZE OneWire::Device::writeBytes(const uint8_t* data, int len, bool keepLocked) const
{
    bus->writeBytes(data, len, keepLocked);
}

bool ONEWIRE_OPTIMIZE OneWire::Device::readBit(bool keepLocked) const
{
    return bus->readBit(keepLocked);
}

uint8_t ONEWIRE_OPTIMIZE OneWire::Device::readByte(bool keepLocked) const
{
    return bus->readByte(keepLocked);
}

void ONEWIRE_OPTIMIZE OneWire::Device::readBytes(uint8_t* data, int len, bool keepLocked) const
{
    return bus->readBytes(data, len, keepLocked);
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
