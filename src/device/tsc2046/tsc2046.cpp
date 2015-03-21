#include "global.h"
#include "device/tsc2046/tsc2046.h"
#include "sys/util.h"


TSC2046::TSC2046(const SPI::Bus* bus, GPIO::Pin cspin, int frequency, int cycles, int threshold)
    : Device(bus, cspin, frequency), cycles(cycles), threshold(threshold)
{
    update();
}

bool TSC2046::update()
{
    select();
    pushByte(0xb8);
    uint16_t oldpress = pressure;
    pressure = pushByte(0) << 8;
    if (!pressure)
    {
        deselect();
        return false;
    }
    uint16_t x;
    uint16_t y;
    uint16_t xprev;
    uint16_t yprev;
    uint32_t xsum = 0;
    uint32_t ysum = 0;
    for (int i = 0; i < cycles; i++)
    {
        pushByte(0xd0);
        x = pushByte(0) << 5;
        x |= pushByte(0x90) >> 3;
        y = pushByte(0) << 5;
        y |= pushByte(0) >> 3;
        if (i && (ABS(x - xprev) > 200 || ABS(y - yprev) > 200)) return !!oldpress;
        xprev = x;
        yprev = y;
        xsum += x;
        ysum += y;
    }
    deselect();
    uint32_t scale = 0x100000 / cycles;
    this->x = (xsum * scale) >> 16;
    this->y = (ysum * scale) >> 16;
    return true;
}
