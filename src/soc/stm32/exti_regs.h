#pragma once

struct __attribute__((packed)) STM32_EXTI_REG_TYPE
{
    uint32_t IMR;
    uint32_t EMR;
    uint32_t RTSR;
    uint32_t FTSR;
    uint32_t SWIER;
    uint32_t PR;
};

#if defined(SOC_STM32F0) || defined(SOC_STM32F1)
#define STM32_EXTI_REGS (*((volatile STM32_EXTI_REG_TYPE*)0x40010400))
#else
#define STM32_EXTI_REGS (*((volatile STM32_EXTI_REG_TYPE*)0x40013c00))
#endif
