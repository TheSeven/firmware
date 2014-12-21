#pragma once

struct __attribute__((packed)) STM32_IWDG_REG_TYPE
{
    uint32_t KR;
    uint32_t PR;
    uint32_t RLR;
    union SR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t PVU : 1;
            uint32_t RVU : 1;
            uint32_t : 30;
        } b;
    } SR;
};

#define STM32_IWDG_REGS (*((volatile STM32_IWDG_REG_TYPE*)0x40003000))
