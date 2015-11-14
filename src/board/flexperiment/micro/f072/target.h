#pragma once

#define BOARD_FLEXPERIMENT_MICRO
#define SOC_STM32F072
#define SOC_STM32F0XXX8
#ifndef STM32_VOLTAGE
#define STM32_VOLTAGE 3300
#endif
#ifndef FLEXPERIMENT_CUSTOM_CLOCKING
#define STM32_CLOCKSOURCE_HSI48
#define STM32_NO_PLL
#endif
#include "soc/stm32/f0/target.h"
