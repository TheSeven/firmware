#pragma once

#include "global.h"


namespace STM32
{
    class __attribute__((packed,aligned(4))) PWR final
    {
    public:
        static void init();
    };
}

