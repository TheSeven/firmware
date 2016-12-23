#pragma once

#include "global.h"


namespace STM32
{
    class __attribute__((packed,aligned(4))) RNG final
    {
    public:
        static bool power(bool on);
        static uint32_t random();
    };
}
