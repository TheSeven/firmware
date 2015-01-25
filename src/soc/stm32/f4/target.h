#pragma once

#define SOC_STM32F4
#if defined(SOC_STM32F4XXXE)
#ifdef STM32_USE_INOFFICIAL
#define FLASH_SIZE 1M
#else
#define FLASH_SIZE 512K
#endif
#define SRAM_SIZE 128K
#elif defined(SOC_STM32F4XXXG) && !defined(SOC_STM32F42X)
#define FLASH_SIZE 1M
#define SRAM_SIZE 128K
#elif defined(SOC_STM32F4XXXG)
#ifdef STM32_USE_INOFFICIAL
#define FLASH_SIZE 2M
#else
#define FLASH_SIZE 1M
#endif
#define SRAM_SIZE 192K
#elif defined(SOC_STM32F4XXXI)
#define FLASH_SIZE 2M
#define SRAM_SIZE 192K
#else
#error Unknown STM32F4 variant!
#endif
#define CCMRAM_SIZE 64K
#define STM32_CAN_BOOT_FROM_RAM
#ifdef SOC_STM32F42X
#define DFU_RAM_SIZE 12K
#else
#define DFU_RAM_SIZE 8K
#endif
#include "soc/stm32/f2/clockgates.h"
#include "soc/stm32/f2/resetlines.h"
#define STM32_GPIO_DEF_FILE "soc/stm32/f2/gpio_defs.h"

#define STM32_HSI_FREQUENCY 16000000
#ifndef STM32_VOLTAGE_SCALE
#define STM32_VOLTAGE_SCALE 1
#endif

#ifdef SOC_STM32F42X
    #ifdef STM32_OVERDRIVE
        #if STM32_VOLTAGE_SCALE == 1
            #define STM32_MAX_SYS_CLOCK 180000000
        #elif STM32_VOLTAGE_SCALE == 2
            #define STM32_MAX_SYS_CLOCK 168000000
        #else
            #error Bad STM32 voltage scale value!
        #endif
        #define STM32_MAX_AHB_CLOCK 180000000
        #define STM32_MAX_APB1_CLOCK 45000000
        #define STM32_MAX_APB2_CLOCK 90000000
        #define STM32_MAX_ADC_CLOCK 36000000
    #else
        #if STM32_VOLTAGE_SCALE == 1
            #define STM32_MAX_SYS_CLOCK 168000000
        #elif STM32_VOLTAGE_SCALE == 2
            #define STM32_MAX_SYS_CLOCK 144000000
        #elif STM32_VOLTAGE_SCALE == 3
            #define STM32_MAX_SYS_CLOCK 120000000
        #else
            #error Bad STM32 voltage scale value!
        #endif
        #define STM32_MAX_AHB_CLOCK 168000000
        #define STM32_MAX_APB1_CLOCK 42000000
        #define STM32_MAX_APB2_CLOCK 84000000
        #define STM32_MAX_ADC_CLOCK 36000000
    #endif
#else
    #if STM32_VOLTAGE_SCALE == 1
        #define STM32_MAX_SYS_CLOCK 168000000
    #elif STM32_VOLTAGE_SCALE == 2
        #define STM32_MAX_SYS_CLOCK 144000000
    #else
        #error Bad STM32 voltage scale value!
    #endif
    #define STM32_MAX_AHB_CLOCK 168000000
    #define STM32_MAX_APB1_CLOCK 42000000
    #define STM32_MAX_APB2_CLOCK 84000000
    #define STM32_MAX_ADC_CLOCK 36000000
#endif

#include "soc/stm32/target.h"

#define CORTEXM_IRQ_DEF_FILE "soc/stm32/f1/irq_defs.h"

#define __FPU_PRESENT 1
#define __NVIC_PRIO_BITS 4
#include "cpu/arm/cortexm/m4/target.h"

