#pragma once

struct __attribute__((packed)) STM32_RNG_REG_TYPE
{
    union CR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t : 2;
            uint32_t RNGEN : 1;
            uint32_t IE : 1;
            uint32_t : 28;
        } b;
    } CR;
    union SR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t DRDY : 1;
            uint32_t CECS : 1;
            uint32_t SECS : 1;
            uint32_t : 2;
            uint32_t CEIS : 1;
            uint32_t SEIS : 1;
            uint32_t : 25;
        } b;
    } SR;
    uint32_t DR;
};

#define STM32_RNG_REGS (*((volatile STM32_RNG_REG_TYPE*)0x50060800))
