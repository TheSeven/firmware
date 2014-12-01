#pragma once

#include "global.h"
#include "interface/spi/spi.h"


namespace STM32
{

    class __attribute__((packed,aligned(4))) SPI final : public ::SPI::Bus
    {
    private:
        uint8_t index;
        constexpr SPI(int index) : index(index) {}

    public:
        int setFrequency(int frequency) const;
        void select() const;
        void deselect() const;
        uint8_t pushByte(uint8_t byte) const;
        void pushBuffer(const void* inbuf, void* outbuf, int len) const;

        static const SPI SPI1;
        static const SPI SPI2;
        static const SPI SPI3;
        static const SPI SPI4;
        static const SPI SPI5;
        static const SPI SPI6;
    };

}

