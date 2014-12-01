#pragma once

#include "global.h"

namespace STM32
{
    class __attribute__((packed,aligned(4))) SYSCFG final
    {
    public:
        static void init();

#ifdef STM32_ENABLE_ETHERNET
        static void setMIIMode(bool rmii);
#endif
    };
}

