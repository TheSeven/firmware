#include "global.h"
#include "soc/stm32/flash.h"
#include "soc/stm32/flash_regs.h"
#include "sys/util.h"


#define STM32_FLASH_WAITSTATES (((STM32_AHB_CLOCK) - 1) / (STM32_FLASH_SPEED))
#if STM32_FLASH_WAITSTATES > 15
#error STM32 AHB bus frequency is too high for this operating voltage!
#elif !defined(SOC_STM32F42X) && STM32_FLASH_WAITSTATES > 7
#error STM32 AHB bus frequency is too high for this operating voltage!
#elif defined(SOC_STM32F0) && STM32_FLASH_WAITSTATES > 1
#error STM32 AHB bus frequency is too high for this operating voltage!
#endif


namespace STM32
{

    void FLASH::init()
    {
        // Configure number of waitstates correctly
        union STM32_FLASH_REG_TYPE::ACR ACR = { 0 };
#if defined(SOC_STM32F0) || defined(SOC_STM32F1)
#ifndef STM32_FLASH_DISABLE_PREFETCH
        ACR.b.PRFTBE = true;
#endif
#else
        ACR.b.DCEN = true;
        ACR.b.ICEN = true;
#ifdef STM32_FLASH_PREFETCH
        ACR.b.PRFTEN = true;
#endif
#endif
        ACR.b.LATENCY = STM32_FLASH_WAITSTATES;
        STM32_FLASH_REGS.ACR.d32 = ACR.d32;
    }

}

