#pragma once

#include "global.h"
#include "interface/gpio/gpio.h"
#include "interface/spi/spi.h"


class __attribute__((packed,aligned(4))) PCD8544 final : SPI::Device
{
public:
    PCD8544(const SPI::Bus* bus, GPIO::Pin cspin, int frequency, GPIO::Pin cd, uint8_t bias, uint8_t tc, uint8_t vop);
    void init();
    void powerDown();
    void blank(bool blank, bool invert);
    void setVop(int vop);
    void setBias(int bias);
    void setTc(int tc);
    void clear();
    int putch(int row, int col, const uint8_t* font, uint8_t invert, char c);
    int print(int row, int col, const uint8_t* font, uint8_t invert, const char* text);
    int printf(int row, int col, const uint8_t* font, uint8_t invert, const char* format, ...);
    int blit(int row, int col, uint8_t* data, int len);
    static const uint8_t defaultFont[];

private:
    struct PrintfState
    {
        PCD8544* obj;
        const uint8_t* font;
        uint8_t invert;
        uint8_t col;
    };

    void sendCmd(bool ext, uint8_t cmd);
    bool jump(int row, int col);
    int drawChar(int col, const uint8_t* font, uint8_t invert, char c);
    static int printfChar(void* state, uint8_t data);

    SPI::Device* spi;
    GPIO::Pin cd;
    uint8_t bias;
    uint8_t tc;
    uint8_t vop;
};
