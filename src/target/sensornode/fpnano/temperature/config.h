#pragma once

#define TIME_OPTIMIZE __attribute__((optimize("-Os")))
#define GPIO_OPTIMIZE __attribute__((optimize("-Os")))
#define SPI_OPTIMIZE __attribute__((optimize("-Os")))
#define STM32_GPIO_OPTIMIZE __attribute__((optimize("-Os")))
#define STM32_SPI_OPTIMIZE __attribute__((optimize("-Os")))
#define STM32_RCC_OPTIMIZE __attribute__((optimize("-Os")))
#define CORTEX_SYSTICK_OPTIMIZE __attribute__((optimize("-Os")))
#define CORTEXUTIL_OPTIMIZE __attribute__((optimize("-Os")))
#define SPIFLASH_OPTIMIZE __attribute__((optimize("-Os")))
#define STORAGEPARTITION_OPTIMIZE __attribute__((optimize("-Os")))
#define FLASHPARTITION_OPTIMIZE __attribute__((optimize("-Os")))
#define CONFIGSTORE_OPTIMIZE __attribute__((optimize("-Os")))
#define ONEWIRE_OPTIMIZE __attribute__((optimize("-Os")))
#define DS1820_OPTIMIZE __attribute__((optimize("-Os")))
#define CRC32_OPTIMIZE __attribute__((optimize("-Os")))
#define NRF_OPTIMIZE __attribute__((optimize("-Os")))
#define NRFBEACON_OPTIMIZE __attribute__((optimize("-Os")))
#define SENSORNODE_CORE_OPTIMIZE __attribute__((optimize("-Os")))
#define SENSORNODE_RADIO_OPTIMIZE __attribute__((optimize("-Os")))
#define SENSORNODE_STORAGE_OPTIMIZE __attribute__((optimize("-Os")))
#define SENSORNODE_SENSOR_OPTIMIZE __attribute__((optimize("-Os")))
#define SENSORNODE_RTC_OPTIMIZE __attribute__((optimize("-Os")))
#define SENSORNODE_TARGET_OPTIMIZE __attribute__((optimize("-Os")))
#define GPIO_SUPPORT_FAST_MODE

#include "../config.h"
