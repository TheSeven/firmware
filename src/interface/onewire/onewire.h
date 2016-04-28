#pragma once

#include "global.h"
#include "interface/gpio/gpio.h"


#ifndef ONEWIRE_OPTIMIZE
#define ONEWIRE_OPTIMIZE
#endif


namespace OneWire
{

    class __attribute__((packed,aligned(4))) Bus
    {
    public:
        Bus(GPIO::Pin pin);
        void sleep() const;
        bool reset() const;
        void select(uint64_t deviceId) const;
        void selectAll() const;
        void writeBit(bool bit, bool keepLocked) const;
        void writeByte(uint8_t byte, bool keepLocked) const;
        void writeBytes(const uint8_t* data, int len, bool keepLocked) const;
        bool readBit(bool keepLocked) const;
        uint8_t readByte(bool keepLocked) const;
        void readBytes(uint8_t* data, int len, bool keepLocked) const;
        void turnOnPower() const;
        void turnOffPower() const;
        void resetDiscovery();
        uint64_t* discoverDevice();

    private:
        static int delay(int time);

        uint64_t scanDeviceId;
        uint8_t lastDiscrepancy;
        bool isLastDevice;
        GPIO::Pin pin;
    };

    class __attribute__((packed,aligned(4))) Device
    {
    protected:
        constexpr Device(const Bus* bus, uint64_t deviceId) : bus(bus), deviceId(deviceId) {}
        void sleep() const;
        void select() const;
        void writeBit(bool bit, bool keepLocked) const;
        void writeByte(uint8_t byte, bool keepLocked) const;
        void writeBytes(const uint8_t* data, int len, bool keepLocked) const;
        bool readBit(bool keepLocked) const;
        uint8_t readByte(bool keepLocked) const;
        void readBytes(uint8_t* data, int len, bool keepLocked) const;
        void turnOnPower() const;
        void turnOffPower() const;
        const uint64_t* getDeviceId() const;

        const Bus* bus;
        uint64_t deviceId;
    };

}
