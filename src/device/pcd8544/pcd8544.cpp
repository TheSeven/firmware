#include "global.h"
#include "device/pcd8544/pcd8544.h"
#include "sys/time.h"
#include "sys/util.h"
#include "lib/printf/printf.h"


PCD8544::PCD8544(const SPI::Bus* bus, GPIO::Pin cspin, int frequency, GPIO::Pin cd, uint8_t bias, uint8_t tc, uint8_t vop)
    : Device(bus, cspin, frequency), cd(cd), bias(bias), tc(tc), vop(vop)
{
    GPIO::configure(cd, GPIO::MODE_OUTPUT, true);
    init();
}

void PCD8544::sendCmd(bool ext, uint8_t cmd)
{
    select();
    GPIO::setLevel(cd, false);
    pushByte(0x20 | ext);
    pushByte(cmd);
    pushByte(0x20);
    GPIO::setLevel(cd, true);
    deselect();
}

void PCD8544::init()
{
    setBias(bias);
    setTc(tc);
    setVop(vop);
    clear();
    blank(false, false);
}

void PCD8544::powerDown()
{
    sendCmd(true, 0x24);
}

void PCD8544::blank(bool blank, bool invert)
{
    sendCmd(false, 0x08 | ((!blank) << 2) | (!!invert));
}

void PCD8544::setVop(int vop)
{
    this->vop = vop;
    sendCmd(true, 0x80 | vop);
}

void PCD8544::setBias(int bias)
{
    this->bias = bias;
    sendCmd(true, 0x10 | bias);
}

void PCD8544::setTc(int tc)
{
    this->tc = tc;
    sendCmd(true, 0x04 | tc);
}

void PCD8544::clear()
{
    select();
    for (int i = 0; i < 6 * 84; i++) pushByte(0);
    deselect();
}

bool PCD8544::jump(int row, int col)
{
    if (col < 0 || col > 83 || row < 0 || row > 5) return false;
    select();
    GPIO::setLevel(cd, false);
    pushByte(0x40 | row);
    pushByte(0x80 | col);
    GPIO::setLevel(cd, true);
    return true;
}

int PCD8544::drawChar(int col, const Fonts::Prop8::Font* font, uint8_t invert, char c)
{
    while (font[1] && (font[0] > c || font[1] < c)) font += 4 + font[1] - font[0] + font[font[1] - font[0] + 3];
    if (!font[1]) return col;
    int width = MIN(84 - col, font[3 + c - font[0]] - font[2 + c - font[0]]);
    col += width;
    font += font[1] - font[0] + 4 + font[2 + c - font[0]];
    while (width--) pushByte(*font++ ^ invert);
    if (col < 84)
    {
        pushByte(invert);
        col++;
    }
    return col;
}

int PCD8544::putch(int row, int col, const Fonts::Prop8::Font* font, uint8_t invert, char c)
{
    if (!jump(row, col)) return col;
    col = drawChar(col, font, invert, c);
    deselect();
    return col;
}

int PCD8544::print(int row, int col, const Fonts::Prop8::Font* font, uint8_t invert, const char* text)
{
    if (!jump(row, col) || !text || !*text) return col;
    char c;
    while ((c = *text++) && col < 84) col = drawChar(col, font, invert, c);
    deselect();
    return col;
}

int PCD8544::printfChar(void* state, uint8_t data)
{
    struct PrintfState* printfState = (struct PrintfState*)state;
    printfState->col = printfState->obj->drawChar(printfState->col, printfState->font, printfState->invert, data);
    return printfState->col < 84;
}

int PCD8544::printf(int row, int col, const Fonts::Prop8::Font* font, uint8_t invert, const char* format, ...)
{
    if (!format || !*format) return col;
    struct PrintfState printfState;
    printfState.obj = this;
    printfState.font = font;
    printfState.invert = invert;
    printfState.col = col;
    va_list args;
    va_start(args, format);
    if (!jump(row, col)) return col;
    printf_format(&printfChar, &printfState, format, args);
    deselect();
    va_end(args);
    return printfState.col;
}

int PCD8544::blit(int row, int col, const uint8_t* data, int len)
{
    if (!jump(row, col)) return col;
    col += len;
    pushBuffer(data, NULL, len);
    deselect();
    return col;
}
