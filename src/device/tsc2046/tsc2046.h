#pragma once

#include "global.h"
#include "interface/spi/spi.h"
#include "interface/touchpanel/touchpanel.h"


class __attribute__((packed,aligned(4))) TSC2046 final : SPI::Device, public TouchPanel
{
public:
    TSC2046(const SPI::Bus* bus, GPIO::Pin cspin, int frequency, int cycles, int threshold);
    virtual bool update();

private:
    uint8_t cycles;
    uint8_t threshold;
};
