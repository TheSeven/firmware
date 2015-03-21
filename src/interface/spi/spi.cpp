#include "global.h"
#include "interface/spi/spi.h"
#include "sys/util.h"


SPI::Device::Device(const Bus* bus, GPIO::Pin cspin, int frequency) : bus(bus), frequency(frequency), deselbytes(0), cspin(cspin)
{
    GPIO::configure(cspin, GPIO::MODE_OUTPUT, true);
}

void SPI::Device::select()
{
    bus->select();
    bus->setFrequency(frequency);
    GPIO::setLevel(cspin, false);
}

void SPI::Device::deselect()
{
    GPIO::setLevel(cspin, true);
    bus->pushBuffer(NULL, NULL, deselbytes);
    bus->deselect();
}

void SPI::Device::reselect()
{
    GPIO::setLevel(cspin, true);
    GPIO::setLevel(cspin, false);
}

uint8_t SPI::Device::pushByte(uint8_t byte)
{
    return bus->pushByte(byte);
}

void SPI::Device::pushBuffer(const void* inbuf, void* outbuf, int len)
{
    return bus->pushBuffer(inbuf, outbuf, len);
}
