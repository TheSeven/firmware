#pragma once

#include "global.h"

struct __attribute__((packed)) STM32_CRC_REG_TYPE
{
    uint32_t DR;
    uint32_t IDR;
    union CR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t RESET : 1;
            uint32_t : 31;
        } b;
    } CR;
};

#define STM32_CRC_REGS (*((volatile STM32_CRC_REG_TYPE*)0x40023000))
