#pragma once

#include "global.h"
#include "interface/display/addressabledisplay.h"
#include "lib/printf/printf.h"


namespace Fonts
{
    class Renderer
    {
    public:
        int putch(int row, int col, int maxcol, bool fill, uint32_t fg, uint32_t bg, char c);
        int print(int row, int col, int maxcol, bool fill, uint32_t fg, uint32_t bg, const char* text);
        int printf(int row, int col, int maxcol, bool fill, uint32_t fg, uint32_t bg, const char* format, ...);

    protected:
        virtual int init(int row, char col, int maxcol) const = 0;
        virtual int drawChar(int cols, char c, uint32_t fg, uint32_t bg) const = 0;
        virtual int finish(int cols, bool fill, uint32_t color) const = 0;

    private:
        struct PrintfState
        {
            Renderer* obj;
            int cols;
            uint32_t fg;
            uint32_t bg;
        };

        static int printfChar(void* state, uint8_t data);
    };
}
