#pragma once

#include "global.h"
#include "interface/gpio/gpio.h"


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
        void writeBit(bool bit) const;
        void writeByte(uint8_t byte) const;
        void writeBytes(const uint8_t* data, int len) const;
        bool readBit() const;
        uint8_t readByte() const;
        void readBytes(uint8_t* data, int len) const;
        void turnOnPower() const;
        void turnOffPower() const;
        void resetDiscovery();
        uint64_t* discoverDevice();
        static uint8_t crc8(const uint8_t* data, int len) __attribute__((pure));

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
        void writeBit(bool bit) const;
        void writeByte(uint8_t byte) const;
        void writeBytes(const uint8_t* data, int len) const;
        bool readBit() const;
        uint8_t readByte() const;
        void readBytes(uint8_t* data, int len) const;
        void turnOnPower() const;
        void turnOffPower() const;
        const uint64_t* getDeviceId() const;

        const Bus* bus;
        uint64_t deviceId;
    };

}
