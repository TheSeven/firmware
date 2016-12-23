#pragma once

#define SOC_STM32F0
#if defined(SOC_STM32F030)
#define FLASH_PAGESIZE 1024
#if defined(SOC_STM32F0XXX4)
#define FLASH_SIZE 16384
#define SRAM_SIZE 4096
#elif defined(SOC_STM32F0XXX6)
#define FLASH_SIZE 32768
#define SRAM_SIZE 4096
#elif defined(SOC_STM32F0XXX8)
#define FLASH_SIZE 65536
#define SRAM_SIZE 8192
#else
#error Unknown STM32F030 variant!
#endif
#elif defined(SOC_STM32F042)
#define STM32_CPUID_ADDR 0x1ffff7ac
#define FLASH_PAGESIZE 1024
#if defined(SOC_STM32F0XXX4)
#ifdef STM32_USE_INOFFICIAL
#define FLASH_SIZE 32768
#else
#define FLASH_SIZE 16384
#endif
#define SRAM_SIZE 4096
#elif defined(SOC_STM32F0XXX6)
#define FLASH_SIZE 32768
#define SRAM_SIZE 4096
#else
#error Unknown STM32F042 variant!
#endif
#elif defined(SOC_STM32F072)
#define STM32_CPUID_ADDR 0x1ffff7ac
#define FLASH_PAGESIZE 2048
#if defined(SOC_STM32F0XXX8)
#ifdef STM32_USE_INOFFICIAL
#define FLASH_SIZE 131072
#else
#define FLASH_SIZE 65536
#endif
#define SRAM_SIZE 16384
#elif defined(SOC_STM32F0XXXC)
#define FLASH_SIZE 131072
#define SRAM_SIZE 16384
#else
#error Unknown STM32F072 variant!
#endif
#else
#error Unknown STM32F0 variant!
#endif
#include "soc/stm32/f0/clockgates.h"
#include "soc/stm32/f0/resetlines.h"
#define STM32_GPIO_DEF_FILE "soc/stm32/f0/gpio_defs.h"

#ifndef STACK_SIZE
#define STACK_SIZE 0x200
#endif

#define STM32_HSI_FREQUENCY 8000000
#define STM32_HSI48_FREQUENCY 48000000
#define STM32_MAX_SYS_CLOCK 48000000
#define STM32_MAX_AHB_CLOCK 48000000
#define STM32_MAX_APB1_CLOCK 48000000
#define STM32_MAX_APB2_CLOCK 48000000
#define STM32_MAX_ADC_CLOCK 14000000
#include "soc/stm32/target.h"

#define CORTEXM_IRQ_DEF_FILE "soc/stm32/f0/irq_defs.h"

#define __NVIC_PRIO_BITS 2
#include "cpu/arm/cortexm/m0/target.h"
