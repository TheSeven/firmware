#include "global.h"
#include "interface/display/addressabledisplay.h"

void Display::AddressableDisplay::blit(int row, int col, int height, int width, void* data)
{
    setDirection({false, false, false});
    setRange(row, col, height, width);
    writeData(data, width * height);
}

void Display::AddressableDisplay::blitExt(int row, int col, int height, int width, void* data, ColorFormat colorFormat, int stride)
{
    setDirection({false, false, false});
    setRange(row, col, height, width);
    while (height--)
    {
        //TODO: Take care of color formats
        writeData(data, width);
        data = (void*)(((int)(data)) + stride * bytesPerPixel);
    }
}

void Display::AddressableDisplay::fill(int row, int col, int height, int width, uint32_t color)
{
    setRange(row, col, height, width);
    writeColor(color, width * height);
}

void Display::AddressableDisplay::setPixel(int row, int col, uint32_t color)
{
    setRange(row, col, 1, 1);
    writeColor(color, 1);
}
