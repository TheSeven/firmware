#pragma once

#include "global.h"
#include "lib/fonts/renderer.h"


namespace Fonts
{
    namespace Prop8
    {
        typedef uint8_t Font;

        class Renderer : public Fonts::Renderer
        {
        public:
            constexpr Renderer(Display::AddressableDisplay* display, const Font* font) : display(display), font(font) { }

        protected:
            virtual int init(int row, char col, int maxcol) const;
            virtual int drawChar(int cols, char c, uint32_t fg, uint32_t bg) const;
            virtual int finish(int cols, bool fill, uint32_t color) const;

        private:
            Display::AddressableDisplay* display;
            const Font* font;
        };

        extern const Font small[];
    }
}
