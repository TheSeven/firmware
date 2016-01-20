#pragma once

#include "global.h"
#include "interface/spi/spi.h"
#include "interface/display/addressabledisplay.h"


class __attribute__((packed,aligned(4))) HX8347 final : SPI::Device, public Display::AddressableDisplay
{
public:
    HX8347(const SPI::Bus* bus, GPIO::Pin cspin, int frequency);
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
    bool inDataMode;
    ::Display::DisplayMode currentMode;
    AddressableDisplay::Direction direction;

    void enterDataMode();
    void powerUp();
    void selectReg(uint8_t cmd, bool read);
    void writeReg(uint8_t cmd, uint8_t data);
    uint8_t readReg(uint8_t cmd);
};
