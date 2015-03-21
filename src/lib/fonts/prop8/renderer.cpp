#include "global.h"
#include "lib/fonts/prop8/prop8.h"
#include "sys/util.h"


int Fonts::Prop8::Renderer::init(int row, char col, int maxcol) const
{
    int cols = MIN(display->width, maxcol) - col;
    if (cols <= 0) return 0;
    display->select();
    display->setDirection({true, false, false});
    display->setRange(row, col, 8, cols);
    return cols;
}

int Fonts::Prop8::Renderer::drawChar(int cols, char c, uint32_t fg, uint32_t bg) const
{
    const Fonts::Prop8::Font* ptr = font;
    while (ptr[1] && (ptr[0] > c || ptr[1] < c)) ptr += 4 + ptr[1] - ptr[0] + ptr[ptr[1] - ptr[0] + 3];
    if (!ptr[1]) return cols;
    int width = MIN(cols, ptr[3 + c - ptr[0]] - ptr[2 + c - ptr[0]]);
    cols -= width;
    ptr += ptr[1] - ptr[0] + 4 + ptr[2 + c - ptr[0]];
    while (width--)
    {
        uint8_t col = *ptr++;
        for (int i = 0; i < 8; i++)
        {
            display->writeColor(col & 1 ? fg : bg, 1);
            col >>= 1;
        }
    }
    if (cols)
    {
        display->writeColor(bg, 8);
        cols--;
    }
    return cols;
}

int Fonts::Prop8::Renderer::finish(int cols, bool fill, uint32_t color) const
{
    if (fill && cols) display->writeColor(color, cols * 8);
    display->deselect();
    return fill ? 0 : cols;
}
