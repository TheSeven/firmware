#include "global.h"
#include "device/hd44780/hd44780.h"
#include "sys/util.h"
#include "sys/time.h"
#include "lib/printf/printf.h"


HD44780::HD44780(GPIO::Pin d0, GPIO::Pin d1, GPIO::Pin d2, GPIO::Pin d3, GPIO::Pin rs, GPIO::Pin en)
    : d{d0, d1, d2, d3}, rs(rs), en(en)
{
    GPIO::configure(en, GPIO::MODE_OUTPUT, false);
    GPIO::configure(rs, GPIO::MODE_OUTPUT);
    for (int i = 0; i < 4; i++) GPIO::configure(d[i], GPIO::MODE_OUTPUT);
    init();
}

void HD44780::sendByte(bool cmd, uint8_t data)
{
    GPIO::setLevel(rs, !cmd);
    for (int i = 4; i--; data <<= 1) GPIO::setLevel(d[i], data & 0x80);
    GPIO::setLevel(en, true);
    GPIO::setLevel(en, false);
    for (int i = 4; i--; data <<= 1) GPIO::setLevel(d[i], data & 0x80);
    GPIO::setLevel(en, true);
    GPIO::setLevel(en, false);
    udelay(10);
}

void HD44780::init()
{
    sendByte(true, 0x33);
    sendByte(true, 0x32);
    sendByte(true, 0x28);
    sendByte(true, 0x04);
    clear();
    mode(false, false, false);
}

void HD44780::mode(bool blank, bool cursor, bool blink)
{
    sendByte(true, 0x08 | (!blank << 2) | (!!cursor << 1) | !!blink);
}

void HD44780::clear()
{
    jump(0, 0);
    sendByte(true, 0x01);
    udelay(2000);
}

void HD44780::jump(int row, int col)
{
    sendByte(true, 0x80 | (row << 6) | col);
}

void HD44780::putch(char c)
{
    sendByte(false, c);
}

int HD44780::print(const char* text)
{
    int i = 0;
    while (char c = text[i++]) sendByte(false, c);
    return i;
}

int HD44780::printfChar(void* state, uint8_t data)
{
    struct PrintfState* printfState = (struct PrintfState*)state;
    printfState->chars++;
    printfState->obj->sendByte(false, data);
    return true;
}

int HD44780::printf(const char* format, ...)
{
    if (!format || !*format) return 0;
    struct PrintfState printfState;
    printfState.obj = this;
    printfState.chars = 0;
    va_list args;
    va_start(args, format);
    printf_format(&printfChar, &printfState, format, args);
    va_end(args);
    return printfState.chars;
}
