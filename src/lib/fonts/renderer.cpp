#include "global.h"
#include "lib/fonts/renderer.h"
#include "sys/util.h"


int Fonts::Renderer::putch(int row, int col, int maxcol, bool fill, uint32_t fg, uint32_t bg, char c)
{
    int cols = init(row, col, maxcol);
    if (!cols) return col;
    col += cols - finish(drawChar(cols, c, fg, bg), fill, bg);
    return col;
}

int Fonts::Renderer::print(int row, int col, int maxcol, bool fill, uint32_t fg, uint32_t bg, const char* text)
{
    if (!text || !*text) return col;
    int cols = init(row, col, maxcol);
    if (!cols) return col;
    col += cols;
    char c;
    while ((c = *text++) && cols) cols = drawChar(cols, c, fg, bg);
    return col - finish(cols, fill, bg);
}

int Fonts::Renderer::printfChar(void* state, uint8_t data)
{
    struct PrintfState* printfState = (struct PrintfState*)state;
    printfState->cols = printfState->obj->drawChar(printfState->cols, data, printfState->fg, printfState->bg);
    return printfState->cols;
}

int Fonts::Renderer::printf(int row, int col, int maxcol, bool fill, uint32_t fg, uint32_t bg, const char* format, ...)
{
    if (!format || !*format) return col;
    struct PrintfState printfState;
    printfState.cols = init(row, col, maxcol);
    if (!printfState.cols) return col;
    col += printfState.cols;
    printfState.obj = this;
    printfState.fg = fg;
    printfState.bg = bg;
    va_list args;
    va_start(args, format);
    printf_format(&printfChar, &printfState, format, args);
    va_end(args);
    return col - finish(printfState.cols, fill, bg);
}
