#pragma once

#define BOARD_FLEXPERIMENT_MINI
#define SOC_STM32F030
#define SOC_STM32F0XXX4
#ifndef STM32_VOLTAGE
#define STM32_VOLTAGE 3300
#endif
#ifndef FLEXPERIMENT_CUSTOM_CLOCKING
#define STM32_CLOCKSOURCE_HSI48
#define STM32_NO_PLL
#endif
#include "soc/stm32/f0/target.h"
