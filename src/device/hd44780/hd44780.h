#pragma once

#include "global.h"
#include "interface/gpio/gpio.h"
#include "lib/printf/printf.h"


class __attribute__((packed,aligned(4))) HD44780 final
{
public:
    HD44780(GPIO::Pin d0, GPIO::Pin d1, GPIO::Pin d2, GPIO::Pin d3, GPIO::Pin rs, GPIO::Pin en);
    void init();
    void mode(bool blank, bool cursor, bool blink);
    void clear();
    void jump(int row, int col);
    void putch(char c);
    int print(const char* text);
    int printf(const char* format, ...);

private:
    struct PrintfState
    {
        HD44780* obj;
        uint8_t chars;
    };

    void sendByte(bool cmd, uint8_t data);
    static int printfChar(void* state, uint8_t data);

    GPIO::Pin d[4], rs, en;
};
