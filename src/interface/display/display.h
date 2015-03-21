#pragma once

#include "global.h"


namespace Display
{
    enum ColorFormat
    {
        RGB565,
    };

    struct DisplayMode
    {
        bool sleep : 1;
        bool lowPowerMode : 1;
        bool blank : 1;
        bool invert : 1;
    };

    class __attribute__((packed,aligned(4))) Display
    {
    public:
        const int width;
        const int height;
        const int bytesPerPixel;
        const ColorFormat colorFormat;

        constexpr Display(int height, int width, int bytesPerPixel, ColorFormat colorFormat)
            : width(width), height(height), bytesPerPixel(bytesPerPixel), colorFormat(colorFormat) { }
        virtual void select() = 0;
        virtual void deselect() = 0;
        virtual void setMode(DisplayMode mode) = 0;
        virtual void setBrightness(int brightness) = 0;
        virtual void setContrast(int contrast) = 0;
        virtual void blit(int row, int col, int height, int width, void* data) = 0;
        virtual void blitExt(int row, int col, int height, int width, void* data, ColorFormat colorFormat, int stride) = 0;
        virtual void fill(int row, int col, int height, int width, uint32_t color) = 0;
        void clear(uint32_t color);
    };

}
