#include "global.h"
#include "interface/spi/spi.h"
#include "sys/util.h"


SPI_OPTIMIZE SPI::Device::Device(const Bus* bus, GPIO::Pin cspin, int frequency)
    : bus(bus), frequency(frequency), deselbytes(0), cspin(cspin)
{
    GPIO::configure(cspin, GPIO::MODE_OUTPUT, true);
}

void SPI_OPTIMIZE SPI::Device::select()
{
    if (selected) return;
#ifdef GPIO_SUPPORT_FAST_MODE
    if (keepBusActive) GPIO::setLevelFast(cspin, false);
#else
    if (keepBusActive) GPIO::setLevel(cspin, false);
#endif
    else
    {
        bus->select();
        bus->setFrequency(frequency);
        GPIO::setLevel(cspin, false);
    }
    selected = true;
}

void SPI_OPTIMIZE SPI::Device::deselect()
{
    if (!selected) return;
#ifdef GPIO_SUPPORT_FAST_MODE
    if (keepBusActive) GPIO::setLevelFast(cspin, true);
#else
    if (keepBusActive) GPIO::setLevel(cspin, true);
#endif
    else
    {
        GPIO::setLevel(cspin, true);
        bus->pushBuffer(NULL, NULL, deselbytes);
        bus->deselect();
    }
    selected = false;
}

void SPI_OPTIMIZE SPI::Device::reselect()
{
#ifdef GPIO_SUPPORT_FAST_MODE
    if (keepBusActive)
    {
        GPIO::setLevelFast(cspin, true);
        GPIO::setLevelFast(cspin, false);
        return;
    }
#endif
    GPIO::setLevel(cspin, true);
    GPIO::setLevel(cspin, false);
}

uint8_t SPI_OPTIMIZE SPI::Device::pushByte(uint8_t byte)
{
    return bus->pushByte(byte);
}

void SPI_OPTIMIZE SPI::Device::pushBuffer(const void* inbuf, void* outbuf, int len)
{
    return bus->pushBuffer(inbuf, outbuf, len);
}

bool SPI_OPTIMIZE SPI::Device::stayAwake(bool on)
{
    if (on)
    {
        if (keepBusActive) return true;
        bus->select();
        bus->setFrequency(frequency);
#ifdef GPIO_SUPPORT_FAST_MODE
        oldGPIOFastState = GPIO::enableFast(cspin, true);
#endif
    }
    else
    {
        if (!keepBusActive) return false;
#ifdef GPIO_SUPPORT_FAST_MODE
        GPIO::enableFast(cspin, oldGPIOFastState);
#endif
        if (!selected)
        {
            bus->pushBuffer(NULL, NULL, deselbytes);
            bus->deselect();
        }
    }
    keepBusActive = on;
    return !on;
}
