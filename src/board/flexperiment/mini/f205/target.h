#pragma once

#define BOARD_FLEXPERIMENT_MINI
#define SOC_STM32F2XXXC
#ifndef STM32_VOLTAGE
#define STM32_VOLTAGE 3300
#endif
#ifndef FLEXPERIMENT_CUSTOM_CLOCKING
#define STM32_CLOCKSOURCE_HSI
#endif
#include "soc/stm32/f2/target.h"
