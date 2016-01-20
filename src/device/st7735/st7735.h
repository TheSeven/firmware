#pragma once

#include "global.h"
#include "interface/spi/spi.h"
#include "interface/display/addressabledisplay.h"


class __attribute__((packed,aligned(4))) ST7735 final : SPI::Device, public Display::AddressableDisplay
{
public:
    ST7735(const SPI::Bus* bus, GPIO::Pin csPin, int frequency, GPIO::Pin cdPin);
    void init();

    virtual void select();
    virtual void deselect();
    virtual void setMode(::Display::DisplayMode mode);
    virtual void setBrightness(int brightness);
    virtual void setContrast(int contrast);
    virtual void setDirection(Direction direction);
    virtual void setRange(int row, int col, int height, int width);
    virtual void writeData(void* data, int pixels);
    virtual void writeColor(uint32_t color, int pixels);

    bool stayAwake(bool on);

private:
    GPIO::Pin cdPin;
    bool inDataMode;
    ::Display::DisplayMode currentMode;
    AddressableDisplay::Direction direction;

    void powerUp();
    void cmd(uint8_t cmd);
};
