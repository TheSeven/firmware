#pragma once

#define SOC_STM32F0
#if defined(SOC_STM32F0XXX4)
#define FLASH_SIZE 16K
#define SRAM_SIZE 4K
#elif defined(SOC_STM32F0XXX6)
#define FLASH_SIZE 32K
#define SRAM_SIZE 4K
#elif defined(SOC_STM32F0XXX8)
#define FLASH_SIZE 64K
#define SRAM_SIZE 8K
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
#define STM32_MAX_SYS_CLOCK 48000000
#define STM32_MAX_AHB_CLOCK 48000000
#define STM32_MAX_APB1_CLOCK 48000000
#define STM32_MAX_APB2_CLOCK 48000000
#define STM32_MAX_ADC_CLOCK 14000000
#include "soc/stm32/target.h"

#define CORTEXM_IRQ_DEF_FILE "soc/stm32/f0/irq_defs.h"

#define __NVIC_PRIO_BITS 2
#include "cpu/arm/cortexm/m0/target.h"
