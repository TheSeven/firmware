#include "global.h"
#include "interface/display/display.h"

void Display::Display::clear(uint32_t color)
{
    fill(0, 0, height, width, color);
}
