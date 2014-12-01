#pragma once

#include "global.h"
#include "interface/onewire/onewire.h"


class __attribute__((packed,aligned(4))) DS1820 final : OneWire::Device
{
public:
    DS1820(const OneWire::Bus* bus, uint64_t deviceId);
    constexpr DS1820() : Device(NULL, 0), externalPower(false), resolution(0) {}
    int readTemperature();
    int readRawTemperature();
    void setResolution(int bits);
    const uint64_t* getDeviceId();

    static int scaleRawTemperature(int data);

private:
    void readScratchpad(uint8_t* data, int len);
    void writeScratchpad(const uint8_t* data);
    void writeEEPROM();
    void readEEPROM();
    void readPowerSupply();
    void convertTemperature();
    void wait(int timeout);

    bool externalPower;
    uint8_t resolution;
};
