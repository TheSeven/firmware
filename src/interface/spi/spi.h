#pragma once

#include "global.h"
#include "interface/gpio/gpio.h"


namespace SPI
{

    class __attribute__((packed,aligned(4))) Bus
    {
    public:
        virtual int setFrequency(int frequency) const = 0;
        virtual void select() const = 0;
        virtual void deselect() const = 0;
        virtual uint8_t pushByte(uint8_t byte) const = 0;
        virtual void pushBuffer(const void* inbuf, void* outbuf, int len) const = 0;
    };

    class __attribute__((packed,aligned(4))) Device
    {
    public:
        const Bus* bus;
        int frequency;
        uint8_t deselbytes;
        GPIO::Pin cspin;

        Device(const Bus* bus, GPIO::Pin cspin, int frequency);
        void select();
        void deselect();
        uint8_t pushByte(uint8_t byte);
        void pushBuffer(const void* inbuf, void* outbuf, int len);
    };

}
