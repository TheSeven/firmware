#pragma once

#define SOC_STM32F1
#ifdef STM32_USE_INOFFICIAL
#define FLASH_SIZE 256K
#else
#if defined(SOC_STM32F1XXXR)
#define FLASH_SIZE 64K
#elif defined(SOC_STM32F1XXXB)
#define FLASH_SIZE 128K
#elif defined(SOC_STM32F1XXXC)
#define FLASH_SIZE 256K
#else
#error Unknown STM32F1 variant!
#endif
#endif
#define SRAM_SIZE 64K
#define STM32_CAN_BOOT_FROM_RAM
#define DFU_RAM_SIZE 4K
#include "soc/stm32/f1/clockgates.h"
#include "soc/stm32/f1/resetlines.h"
#define STM32_GPIO_DEF_FILE "soc/stm32/f1/gpio_defs.h"

#define STM32_HSI_FREQUENCY 8000000
#define STM32_MAX_SYS_CLOCK 72000000
#define STM32_MAX_AHB_CLOCK 72000000
#define STM32_MAX_APB1_CLOCK 72000000
#define STM32_MAX_APB2_CLOCK 36000000
#define STM32_MAX_ADC_CLOCK 14000000
#ifndef STM32_ADC_CLOCK
#define STM32_ADC_CLOCK 12000000
#endif

#define GPIO_STATIC_CONTROLLER_HEADER "soc/stm32/f1/gpio.h"
#define GPIO_STATIC_CONTROLLER STM32::GPIO_F1::Controller

#include "soc/stm32/target.h"

#define CORTEXM_IRQ_DEF_FILE "soc/stm32/f1/irq_defs.h"

#define __NVIC_PRIO_BITS 4
#include "cpu/arm/cortexm/m3/target.h"

