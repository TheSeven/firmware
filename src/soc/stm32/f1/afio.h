#pragma once

#include "global.h"

namespace STM32
{
    class __attribute__((packed,aligned(4))) AFIO final
    {
    public:
        static void init();

        static void setMIIMode(bool rmii);
    };
}

