#pragma once

#include "global.h"
#include "interface/storage/storage.h"


#ifndef STM32FLASH_OPTIMIZE
#define STM32FLASH_OPTIMIZE
#endif


#ifndef STM32_VOLTAGE
    #error Unknown STM32 operating voltage, cannot determine correct configuration
#endif

#if defined(SOC_STM32F42X)
    #if (STM32_VOLTAGE) < 1700
        #error STM32 operating voltage is too low! Minimum is 1700mV.
    #elif (STM32_VOLTAGE) < 2100
        #define STM32_FLASH_SPEED 20000000
    #elif (STM32_VOLTAGE) < 2400
        #define STM32_FLASH_SPEED 22000000
    #elif (STM32_VOLTAGE) < 2700
        #define STM32_FLASH_SPEED 24000000
    #elif (STM32_VOLTAGE) < 3600
        #define STM32_FLASH_SPEED 30000000
    #else
        #error STM32 operating voltage is too high! Maximum is 3600mV.
    #endif
#elif defined(SOC_STM32F4)
    #if (STM32_VOLTAGE) < 1700
        #error STM32 operating voltage is too low! Minimum is 1700mV.
    #elif (STM32_VOLTAGE) < 2100
        #define STM32_FLASH_SPEED 16000000
    #elif (STM32_VOLTAGE) < 2400
        #define STM32_FLASH_SPEED 18000000
    #elif (STM32_VOLTAGE) < 2700
        #define STM32_FLASH_SPEED 24000000
    #elif (STM32_VOLTAGE) < 3600
        #define STM32_FLASH_SPEED 30000000
    #else
        #error STM32 operating voltage is too high! Maximum is 3600mV.
    #endif
#elif defined(SOC_STM32F2)
    #if (STM32_VOLTAGE) < 1700
        #error STM32 operating voltage is too low! Minimum is 1700mV.
    #elif (STM32_VOLTAGE) < 2100
        #define STM32_FLASH_SPEED 16000000
    #elif (STM32_VOLTAGE) < 2400
        #define STM32_FLASH_SPEED 18000000
    #elif (STM32_VOLTAGE) < 2700
        #define STM32_FLASH_SPEED 24000000
    #elif (STM32_VOLTAGE) < 3600
        #define STM32_FLASH_SPEED 30000000
    #else
        #error STM32 operating voltage is too high! Maximum is 3600mV.
    #endif
#elif defined(SOC_STM32F1)
    #if (STM32_VOLTAGE) < 2000
        #error STM32 operating voltage is too low! Minimum is 2000mV.
    #elif (STM32_VOLTAGE) < 3600
        #define STM32_FLASH_SPEED 24000000
    #else
        #error STM32 operating voltage is too high! Maximum is 3600mV.
    #endif
#elif defined(SOC_STM32F072)
    #if (STM32_VOLTAGE) < 2000
        #error STM32 operating voltage is too low! Minimum is 2000mV.
    #elif (STM32_VOLTAGE) < 3600
        #define STM32_FLASH_SPEED 24000000
    #else
        #error STM32 operating voltage is too high! Maximum is 3600mV.
    #endif
#elif defined(SOC_STM32F0)
    #if (STM32_VOLTAGE) < 2400
        #error STM32 operating voltage is too low! Minimum is 2400mV.
    #elif (STM32_VOLTAGE) < 3600
        #define STM32_FLASH_SPEED 24000000
    #else
        #error STM32 operating voltage is too high! Maximum is 3600mV.
    #endif
#else
    #error Unknown SOC architecture, cannot determine correct configuration
#endif

namespace STM32
{
    class __attribute__((packed,aligned(4))) FLASH final
    {
    public:
        static void init();
    };

#if defined(SOC_STM32F0) || defined(SOC_STM32F1)
    class __attribute__((packed,aligned(4))) FlashDriver final : public Storage
    {
        void waitIdle();
        bool enableWrite();
        void disableWrite(bool oldHSI);

    public:
        constexpr FlashDriver() : Storage(FLASH_SIZE, 1, FLASH_PAGESIZE, 2) {}
        virtual enum Result reset();
        virtual enum Result getStatus();
        virtual enum Result read(uint32_t page, uint32_t len, void* buf);
        virtual enum Result write(uint32_t page, uint32_t len, const void* buf);
        virtual enum Result erase(uint32_t page, uint32_t len);
    };

    extern FlashDriver Flash;
#endif
}
