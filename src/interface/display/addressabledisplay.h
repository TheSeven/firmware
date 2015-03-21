#pragma once

#include "global.h"
#include "interface/display/display.h"


namespace Display
{

    class __attribute__((packed,aligned(4))) AddressableDisplay : public Display
    {
    public:
        constexpr AddressableDisplay(int height, int width, int bytesPerPixel, ColorFormat colorFormat)
            : Display(height, width, bytesPerPixel, colorFormat) { }
        void blit(int row, int col, int height, int width, void* data);
        void blitExt(int row, int col, int height, int width, void* data, ColorFormat colorFormat, int stride);
        void fill(int row, int col, int width, int height, uint32_t color);
        void setPixel(int row, int col, uint32_t color);

        struct Direction
        {
            bool vertical : 1;
            bool hInvert : 1;
            bool vInvert : 1;
        };

        virtual void setDirection(Direction direction) = 0;
        virtual void setRange(int row, int col, int height, int width) = 0;
        virtual void writeData(void* data, int pixels) = 0;
        virtual void writeColor(uint32_t color, int pixels) = 0;
    };

}
