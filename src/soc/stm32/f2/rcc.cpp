#include "global.h"
#include "soc/stm32/f2/rcc.h"
#include "soc/stm32/f2/rcc_regs.h"
#include "soc/stm32/pwr.h"
#include "soc/stm32/flash.h"
#include "cpu/arm/cortexm/cortexutil.h"
#include "cpu/arm/cortexm/time.h"
#include "interface/clockgate/clockgate.h"
#include "interface/resetline/resetline.h"
#include "sys/util.h"

#if (STM32_SYS_CLOCK) > (STM32_MAX_SYS_CLOCK)
    #error STM32 SYSCLK bus frequency is too high!
#endif
#if (STM32_AHB_CLOCK) > (STM32_MAX_AHB_CLOCK)
    #error STM32 AHB bus frequency is too high!
#endif
#if (STM32_APB1_CLOCK) > (STM32_MAX_APB1_CLOCK)
    #error STM32 APB1 bus frequency is too high!
#endif
#if (STM32_APB2_CLOCK) > (STM32_MAX_APB2_CLOCK)
    #error STM32 APB2 bus frequency is too high!
#endif

#if defined(STM32_CLOCKSOURCE_HSE) && !defined(STM32_CLOCKSOURCE_HSI)
    #if (STM32_HSE_FREQUENCY) < 4000000
        #error STM32 HSE input frequency is too low! Minimum is 4000000Hz.
    #elif (STM32_HSE_FREQUENCY) > 26000000
        #error STM32 HSE input frequency is too high! Maximum is 26000000Hz.
    #endif
    #if (!defined(STM32_HSE_CRYSTAL) && !defined(STM32_HSE_EXTERNAL)) || (defined(STM32_HSE_CRYSTAL) && defined(STM32_HSE_EXTERNAL))
        #error Invalid STM32 HSE oscillator configuration!
    #endif
    #define STM32_PLL_BYPASS CFGR.b.SW_HSE
    #define STM32_PLL_SOURCE PLLCFGR.b.PLLSRC_HSE
    #define STM32_PLL_SOURCE_FREQUENCY STM32_HSE_FREQUENCY
#elif !defined(STM32_CLOCKSOURCE_HSE) && defined(STM32_CLOCKSOURCE_HSI)
    #define STM32_PLL_BYPASS CFGR.b.SW_HSI
    #define STM32_PLL_SOURCE PLLCFGR.b.PLLSRC_HSI
    #define STM32_PLL_SOURCE_FREQUENCY STM32_HSI_FREQUENCY
#else
#error Invalid STM32 clock source configuration!
#endif

#ifdef STM32_NO_PLL
    #define STM32_SYS_CLOCK STM32_PLL_SOURCE_FREQUENCY
    #define STM32_SYS_SOURCE STM32_PLL_BYPASS
#else
    #define STM32_SYS_SOURCE CFGR.b.SW_PLL

    #ifndef STM32_PLLIN_CLOCK
        #define STM32_PLLIN_CLOCK 2000000
    #endif
    #if (STM32_PLLIN_CLOCK) < 1000000
        #error STM32 PLL input frequency is too low! Minimum is 1000000Hz.
    #elif (STM32_PLLIN_CLOCK) > 2000000
        #error STM32 PLL input frequency is too high! Maximum is 2000000Hz.
    #endif
    #define STM32_PLLIN_DIVIDER ((STM32_PLL_SOURCE_FREQUENCY) / (STM32_PLLIN_CLOCK))
    #if (STM32_PLLIN_CLOCK) * (STM32_PLLIN_DIVIDER) != (STM32_PLL_SOURCE_FREQUENCY)
        #error Can not derive the specified PLL input clock from the specified STM32 input clock!
    #endif

    #ifndef STM32_PLLOUT_CLOCK
        #if (STM32_SYS_CLOCK) * 2 >= 64000000 && (STM32_SYS_CLOCK) * 2 <= 432000000 && (STM32_SYS_CLOCK) * 2 / 48000000 * 48000000 == (STM32_SYS_CLOCK) * 2
            #define STM32_PLLOUT_CLOCK ((STM32_SYS_CLOCK) * 2)
        #elif (STM32_SYS_CLOCK) * 4 >= 64000000 && (STM32_SYS_CLOCK) * 4 <= 432000000 && (STM32_SYS_CLOCK) * 4 / 48000000 * 48000000 == (STM32_SYS_CLOCK) * 4
            #define STM32_PLLOUT_CLOCK ((STM32_SYS_CLOCK) * 4)
        #elif (STM32_SYS_CLOCK) * 6 >= 64000000 && (STM32_SYS_CLOCK) * 6 <= 432000000 && (STM32_SYS_CLOCK) * 6 / 48000000 * 48000000 == (STM32_SYS_CLOCK) * 6
            #define STM32_PLLOUT_CLOCK ((STM32_SYS_CLOCK) * 6)
        #elif (STM32_SYS_CLOCK) * 8 >= 64000000 && (STM32_SYS_CLOCK) * 8 <= 432000000 && (STM32_SYS_CLOCK) * 8 / 48000000 * 48000000 == (STM32_SYS_CLOCK) * 8
            #define STM32_PLLOUT_CLOCK ((STM32_SYS_CLOCK) * 8)
        #else
            #define STM32_PLLOUT_CLOCK 0
        #endif
    #endif
    #if (STM32_PLLOUT_CLOCK) < 64000000
        #error STM32 PLL output frequency is too low! Minimum is 64000000Hz.
    #elif (STM32_PLLOUT_CLOCK) > 432000000
        #error STM32 PLL output frequency is too high! Maximum is 432000000Hz.
    #endif
    #define STM32_PLL_MULTIPLIER ((STM32_PLLOUT_CLOCK) / (STM32_PLLIN_CLOCK))
    #if (STM32_PLLIN_CLOCK) * (STM32_PLL_MULTIPLIER) != (STM32_PLLOUT_CLOCK)
        #error Can not find a PLL multiplication factor for the specified PLL output frequency!
    #endif
    #if (STM32_PLL_MULTIPLIER) < 64
        #error STM32 PLL multiplication factor is too low! Minimum is 64.
    #elif (STM32_PLL_MULTIPLIER) > 432
        #error STM32 PLL multiplication factor is too high! Maximum is 432.
    #endif

    #define STM32_SYS_DIVIDER ((STM32_PLLOUT_CLOCK) / (STM32_SYS_CLOCK) / 2 - 1)
    #if (STM32_SYS_CLOCK) * (STM32_SYS_DIVIDER + 1) * 2 != (STM32_PLLOUT_CLOCK) || (STM32_SYS_DIVIDER) < 0 || (STM32_SYS_DIVIDER) > 3
        #error Can not derive the specified SYSCLK clock from the specified PLL output frequency!
    #endif

    #if ((STM32_PLLOUT_CLOCK) + 47999999) / 48000000 < 2
        #define STM32_48M_DIVIDER 2
    #elif ((STM32_PLLOUT_CLOCK) + 47999999) / 48000000 > 15
        #define STM32_48M_DIVIDER 15
    #else
        #define STM32_48M_DIVIDER (((STM32_PLLOUT_CLOCK) + 47999999) / 48000000)
    #endif
    #if 48000000 * (STM32_48M_DIVIDER) != (STM32_PLLOUT_CLOCK)
        #warning Some STM32 peripherals require the PLL output frequency to be a multiple of 48000000Hz!
    #endif

#endif


#if (STM32_AHB_CLOCK) == (STM32_SYS_CLOCK)
    #define STM32_AHB_DIVIDER CFGR.b.HPRE_DIV1
#elif (STM32_AHB_CLOCK) * 2 == (STM32_SYS_CLOCK)
    #define STM32_AHB_DIVIDER CFGR.b.HPRE_DIV2
#elif (STM32_AHB_CLOCK) * 4 == (STM32_SYS_CLOCK)
    #define STM32_AHB_DIVIDER CFGR.b.HPRE_DIV4
#elif (STM32_AHB_CLOCK) * 8 == (STM32_SYS_CLOCK)
    #define STM32_AHB_DIVIDER CFGR.b.HPRE_DIV8
#elif (STM32_AHB_CLOCK) * 16 == (STM32_SYS_CLOCK)
    #define STM32_AHB_DIVIDER CFGR.b.HPRE_DIV16
#elif (STM32_AHB_CLOCK) * 64 == (STM32_SYS_CLOCK)
    #define STM32_AHB_DIVIDER CFGR.b.HPRE_DIV64
#elif (STM32_AHB_CLOCK) * 128 == (STM32_SYS_CLOCK)
    #define STM32_AHB_DIVIDER CFGR.b.HPRE_DIV128
#elif (STM32_AHB_CLOCK) * 256 == (STM32_SYS_CLOCK)
    #define STM32_AHB_DIVIDER CFGR.b.HPRE_DIV256
#elif (STM32_AHB_CLOCK) * 512 == (STM32_SYS_CLOCK)
    #define STM32_AHB_DIVIDER CFGR.b.HPRE_DIV512
#else
    #error Can not derive the AHB clock from the specified SYSCLK output frequency!
#endif

#if (STM32_APB1_CLOCK) == (STM32_AHB_CLOCK)
    #define STM32_APB1_DIVIDER CFGR.b.PPRE1_DIV1
#elif (STM32_APB1_CLOCK) * 2 == (STM32_AHB_CLOCK)
    #define STM32_APB1_DIVIDER CFGR.b.PPRE1_DIV2
#elif (STM32_APB1_CLOCK) * 4 == (STM32_AHB_CLOCK)
    #define STM32_APB1_DIVIDER CFGR.b.PPRE1_DIV4
#elif (STM32_APB1_CLOCK) * 8 == (STM32_AHB_CLOCK)
    #define STM32_APB1_DIVIDER CFGR.b.PPRE1_DIV8
#elif (STM32_APB1_CLOCK) * 16 == (STM32_AHB_CLOCK)
    #define STM32_APB1_DIVIDER CFGR.b.PPRE1_DIV16
#else
    #error Can not derive the APB1 clock from the specified AHB clock frequency!
#endif

#if (STM32_APB2_CLOCK) == (STM32_AHB_CLOCK)
    #define STM32_APB2_DIVIDER CFGR.b.PPRE2_DIV1
#elif (STM32_APB2_CLOCK) * 2 == (STM32_AHB_CLOCK)
    #define STM32_APB2_DIVIDER CFGR.b.PPRE2_DIV2
#elif (STM32_APB2_CLOCK) * 4 == (STM32_AHB_CLOCK)
    #define STM32_APB2_DIVIDER CFGR.b.PPRE2_DIV4
#elif (STM32_APB2_CLOCK) * 8 == (STM32_AHB_CLOCK)
    #define STM32_APB2_DIVIDER CFGR.b.PPRE2_DIV8
#elif (STM32_APB2_CLOCK) * 16 == (STM32_AHB_CLOCK)
    #define STM32_APB2_DIVIDER CFGR.b.PPRE2_DIV16
#else
    #error Can not derive the APB2 clock from the specified AHB clock frequency!
#endif

namespace STM32
{

    void STM32_RCC_OPTIMIZE RCC::init()
    {
        // Reset all peripherals
        STM32_RCC_REGS.RSTLINES.r.AHB1RSTR.d32 = 0xffffffff;
        STM32_RCC_REGS.RSTLINES.r.AHB1RSTR.d32 = 0;
        STM32_RCC_REGS.RSTLINES.r.AHB2RSTR.d32 = 0xffffffff;
        STM32_RCC_REGS.RSTLINES.r.AHB2RSTR.d32 = 0;
        STM32_RCC_REGS.RSTLINES.r.AHB3RSTR.d32 = 0xffffffff;
        STM32_RCC_REGS.RSTLINES.r.AHB3RSTR.d32 = 0;
        STM32_RCC_REGS.RSTLINES.r.APB1RSTR.d32 = 0xffffffff;
        STM32_RCC_REGS.RSTLINES.r.APB1RSTR.d32 = 0;
        STM32_RCC_REGS.RSTLINES.r.APB2RSTR.d32 = 0xffffffff;
        STM32_RCC_REGS.RSTLINES.r.APB2RSTR.d32 = 0;
        
        // Disable all RCC interrupts
        STM32_RCC_REGS.CIR.d32 = 0;
        
        // Set up sane clock gating defaults
        union STM32_RCC_REG_TYPE::CLKGATES::REG::AHB1ENR AHB1ENR = { 0 };
        AHB1ENR.b.CCMDATARAMEN = true;
        STM32_RCC_REGS.CLKGATES.r.AHB1ENR.d32 = AHB1ENR.d32;
        union STM32_RCC_REG_TYPE::AHB1LPENR AHB1LPENR = { 0 };
        AHB1LPENR.b.CRCLPEN = true;
        AHB1LPENR.b.DMA1LPEN = true;
        AHB1LPENR.b.DMA2LPEN = true;
        AHB1LPENR.b.DMA2DLPEN = true;
        AHB1LPENR.b.ETHMACLPEN = true;
        AHB1LPENR.b.ETHMACTXLPEN = true;
        AHB1LPENR.b.ETHMACRXLPEN = true;
        AHB1LPENR.b.ETHMACPTPLPEN = true;
        AHB1LPENR.b.OTGHSLPEN = true;
        AHB1LPENR.b.OTGHSULPILPEN = true;
        STM32_RCC_REGS.AHB1LPENR.d32 = AHB1LPENR.d32;
        union STM32_RCC_REG_TYPE::CLKGATES::REG::AHB2ENR AHB2ENR = { 0 };
        STM32_RCC_REGS.CLKGATES.r.AHB2ENR.d32 = AHB2ENR.d32;
        union STM32_RCC_REG_TYPE::AHB2LPENR AHB2LPENR = { 0 };
        AHB2LPENR.b.DCMILPEN = true;
        AHB2LPENR.b.CRYPLPEN = true;
        AHB2LPENR.b.HASHLPEN = true;
        AHB2LPENR.b.RNGLPEN = true;
        AHB2LPENR.b.OTGFSLPEN = true;
        STM32_RCC_REGS.AHB2LPENR.d32 = AHB2LPENR.d32;
        union STM32_RCC_REG_TYPE::CLKGATES::REG::AHB3ENR AHB3ENR = { 0 };
        STM32_RCC_REGS.CLKGATES.r.AHB3ENR.d32 = AHB3ENR.d32;
        union STM32_RCC_REG_TYPE::AHB3LPENR AHB3LPENR = { 0 };
        AHB3LPENR.b.FMCLPEN = true;
        STM32_RCC_REGS.AHB3LPENR.d32 = AHB3LPENR.d32;
        union STM32_RCC_REG_TYPE::CLKGATES::REG::APB1ENR APB1ENR = { 0 };
        STM32_RCC_REGS.CLKGATES.r.APB1ENR.d32 = APB1ENR.d32;
        union STM32_RCC_REG_TYPE::APB1LPENR APB1LPENR = { 0 };
        APB1LPENR.b.TIM2LPEN = true;
        APB1LPENR.b.TIM3LPEN = true;
        APB1LPENR.b.TIM4LPEN = true;
        APB1LPENR.b.TIM5LPEN = true;
        APB1LPENR.b.TIM6LPEN = true;
        APB1LPENR.b.TIM7LPEN = true;
        APB1LPENR.b.TIM12LPEN = true;
        APB1LPENR.b.TIM13LPEN = true;
        APB1LPENR.b.TIM14LPEN = true;
        APB1LPENR.b.WWDGLPEN = true;
        APB1LPENR.b.SPI2LPEN = true;
        APB1LPENR.b.SPI3LPEN = true;
        APB1LPENR.b.USART2LPEN = true;
        APB1LPENR.b.USART3LPEN = true;
        APB1LPENR.b.UART4LPEN = true;
        APB1LPENR.b.UART5LPEN = true;
        APB1LPENR.b.I2C1LPEN = true;
        APB1LPENR.b.I2C2LPEN = true;
        APB1LPENR.b.I2C3LPEN = true;
        APB1LPENR.b.CAN1LPEN = true;
        APB1LPENR.b.CAN2LPEN = true;
        APB1LPENR.b.DACLPEN = true;
        APB1LPENR.b.UART7LPEN = true;
        APB1LPENR.b.UART8LPEN = true;
        STM32_RCC_REGS.APB1LPENR.d32 = APB1LPENR.d32;
        union STM32_RCC_REG_TYPE::CLKGATES::REG::APB2ENR APB2ENR = { 0 };
        STM32_RCC_REGS.CLKGATES.r.APB2ENR.d32 = APB2ENR.d32;
        union STM32_RCC_REG_TYPE::APB2LPENR APB2LPENR = { 0 };
        APB2LPENR.b.TIM1LPEN = true;
        APB2LPENR.b.TIM8LPEN = true;
        APB2LPENR.b.USART1LPEN = true;
        APB2LPENR.b.USART6LPEN = true;
        APB2LPENR.b.ADC1LPEN = true;
        APB2LPENR.b.ADC2LPEN = true;
        APB2LPENR.b.ADC3LPEN = true;
        APB2LPENR.b.SDIOLPEN = true;
        APB2LPENR.b.SPI1LPEN = true;
        APB2LPENR.b.SPI4LPEN = true;
        APB2LPENR.b.TIM9LPEN = true;
        APB2LPENR.b.TIM10LPEN = true;
        APB2LPENR.b.TIM11LPEN = true;
        APB2LPENR.b.SPI5LPEN = true;
        APB2LPENR.b.SPI6LPEN = true;
        APB2LPENR.b.SAI1LPEN = true;
        APB2LPENR.b.LTDCLPEN = true;
        STM32_RCC_REGS.APB2LPENR.d32 = APB2LPENR.d32;
        
        // Switch to HSI so that we can configure the PLLs
        STM32_RCC_REGS.CR.b.HSION = true;
        union STM32_RCC_REG_TYPE::CFGR CFGR = { 0 };
        CFGR.b.SW = CFGR.b.SW_HSI;
        STM32_RCC_REGS.CFGR.d32 = CFGR.d32;
        while (STM32_RCC_REGS.CFGR.b.SWS != CFGR.b.SWS_HSI);
        
        // Power down HSE and PLLs
        union STM32_RCC_REG_TYPE::CR CR = { 0 };
        CR.b.HSITRIM = 16;
        CR.b.HSION = true;
        STM32_RCC_REGS.CR.d32 = CR.d32;
        
        // Configure and start HSE
#ifdef STM32_CLOCKSOURCE_HSE
#ifdef STM32_HSE_EXTERNAL
        CR.b.HSEBYP = true;
        STM32_RCC_REGS.CR.d32 = CR.d32;
#endif
        CR.b.HSEON = true;
        STM32_RCC_REGS.CR.d32 = CR.d32;
#endif

#ifndef STM32_NO_PLL
        // Configure and start main PLL
        union STM32_RCC_REG_TYPE::PLLCFGR PLLCFGR = { 0 };
        PLLCFGR.b.PLLSRC = STM32_PLL_SOURCE;
        PLLCFGR.b.PLLM = STM32_PLLIN_DIVIDER;
        PLLCFGR.b.PLLN = STM32_PLL_MULTIPLIER;
        PLLCFGR.b.PLLP = STM32_SYS_DIVIDER;
        PLLCFGR.b.PLLQ = STM32_48M_DIVIDER;
        STM32_RCC_REGS.PLLCFGR.d32 = PLLCFGR.d32;
        STM32_RCC_REGS.CR.b.PLLON = true;
#endif

        // Initialize power management while the PLL is locking
        PWR::init();
        
        // Set flash wait states for higher clock frequency
        FLASH::init();
        
        // Configure system clock source and prescalers
        CFGR.b.SW = STM32_SYS_SOURCE;
        CFGR.b.HPRE = STM32_AHB_DIVIDER;
        CFGR.b.PPRE1 = STM32_APB1_DIVIDER;
        CFGR.b.PPRE2 = STM32_APB2_DIVIDER;
        STM32_RCC_REGS.CFGR.d32 = CFGR.d32;

#ifndef STM32_CLOCKSOURCE_HSI
        // Shut down HSI, we don't need it anymore
        while (STM32_RCC_REGS.CFGR.b.SWS == CFGR.b.SWS_HSI);
        STM32_RCC_REGS.CR.b.HSION = false;
#endif
    }
    
    int STM32_RCC_OPTIMIZE RCC::getPLLOutFrequency()
    {
#ifdef STM32_RCC_DYNAMIC
        union STM32_RCC_REG_TYPE::PLLCFGR PLLCFGR = { STM32_RCC_REGS.PLLCFGR.d32 };
        int baseclock;
        switch (PLLCFGR.b.PLLSRC)
        {
            case PLLCFGR.b.PLLSRC_HSI: baseclock = STM32_HSI_FREQUENCY; break;
            case PLLCFGR.b.PLLSRC_HSE: baseclock = STM32_HSE_FREQUENCY; break;
        }
        baseclock /= PLLCFGR.b.PLLM;
        baseclock *= PLLCFGR.b.PLLN;
        return baseclock;
#else
        return STM32_PLLOUT_CLOCK;
#endif
    }

    int STM32_RCC_OPTIMIZE RCC::getSysClockFrequency()
    {
#ifdef STM32_RCC_DYNAMIC
        int baseclock;
        switch (STM32_RCC_REGS.CFGR.b.SW)
        {
            case STM32_RCC_REGS.CFGR.b.SW_HSI: return STM32_HSI_FREQUENCY;
            case STM32_RCC_REGS.CFGR.b.SW_HSE: return STM32_HSE_FREQUENCY;
            case STM32_RCC_REGS.CFGR.b.SW_PLL:
                baseclock = getPLLOutFrequency();
                baseclock /= STM32_RCC_REGS.PLLCFGR.b.PLLP + 1;
                return baseclock >> 1;
            default: return 0;
        }
#else
        return STM32_SYS_CLOCK;
#endif
    }
    
    int STM32_RCC_OPTIMIZE RCC::getAHBClockFrequency()
    {
#ifdef STM32_RCC_DYNAMIC
        int prescaler = STM32_RCC_REGS.CFGR.b.HPRE;
        int baseclock = getSysClockFrequency();
        if (prescaler & 8) baseclock >>= 1;
        return baseclock >> (prescaler & 7);
#else
        return STM32_AHB_CLOCK;
#endif
    }
    
    int STM32_RCC_OPTIMIZE RCC::getAPBClockFrequency(APBBus bus)
    {
#ifdef STM32_RCC_DYNAMIC
        int prescaler = 0;
        switch (bus)
        {
            case APB1: prescaler = STM32_RCC_REGS.CFGR.b.PPRE1; break;
            case APB2: prescaler = STM32_RCC_REGS.CFGR.b.PPRE2; break;
        }
        int baseclock = getAHBClockFrequency();
        if (prescaler & 4) baseclock >>= 1;
        return baseclock >> (prescaler & 3);
#else
        switch (bus)
        {
            case APB1: return STM32_APB1_CLOCK;
            case APB2: return STM32_APB2_CLOCK;
            default: return 0;
        }
#endif
    }

    int STM32_RCC_OPTIMIZE RCC::get48MClockFrequency()
    {
#ifdef STM32_RCC_DYNAMIC
        return getPLLOutFrequency() / STM32_RCC_REGS.PLLCFGR.b.PLLQ;
#else
        return STM32_PLLOUT_CLOCK / STM32_48M_DIVIDER;
#endif
    }

#ifdef STM32_RCC_DYNAMIC
    void STM32_RCC_OPTIMIZE RCC::setSysClockSource(SysClockSource source)
    {
        enter_critical_section();
        STM32_RCC_REGS.CFGR.b.SW = source;
        while (STM32_RCC_REGS.CFGR.b.SWS != source);
        leave_critical_section();
    }
    
    void STM32_RCC_OPTIMIZE RCC::setAHBDivider(int divider)
    {
        int value = 0;
        if (divider > 1) value = 8;
        if (divider > 2) value++;
        if (divider > 4) value++;
        if (divider > 8) value++;
        if (divider > 16) value++;
        if (divider > 64) value++;
        if (divider > 128) value++;
        if (divider > 256) value++;
        enter_critical_section();
        STM32_RCC_REGS.CFGR.b.HPRE = value;
        leave_critical_section();
    }
    
    void STM32_RCC_OPTIMIZE RCC::setAPBDivider(APBBus bus, int divider)
    {
        int value = 0;
        if (divider > 1) value = 4;
        if (divider > 2) value++;
        if (divider > 4) value++;
        if (divider > 8) value++;
        enter_critical_section();
        switch (bus)
        {
            case APB1: STM32_RCC_REGS.CFGR.b.PPRE1 = value; break;
            case APB2: STM32_RCC_REGS.CFGR.b.PPRE2 = value; break;
        }
        leave_critical_section();
    }
    
    void STM32_RCC_OPTIMIZE RCC::setPLLClockSource(PLLClockSource source, int divider)
    {
        enter_critical_section();
        union STM32_RCC_REG_TYPE::PLLCFGR PLLCFGR.d32 = STM32_RCC_REGS.PLLCFGR.d32;
        PLLCFGR.PLLSRC = source;
        PLLCFGR.PLLM = divider;
        STM32_RCC_REGS.PLLCFGR.d32 = PLLCFGR.d32;
        leave_critical_section();
    }
    
    void STM32_RCC_OPTIMIZE RCC::configurePLL(PLL pll, bool on, int n, int p, int q, int r)
    {
        enter_critical_section();
        union STM32_RCC_REG_TYPE::PLLCFGR PLLCFGR;
        switch (pll)
        {
            case MainPLL:
                PLLCFGR.d32 = STM32_RCC_REGS.PLLCFGR.d32;
                STM32_RCC_REGS.CR.b.PLLON &= on;
                break;
            case PLLI2S:
                PLLCFGR.d32 = STM32_RCC_REGS.PLLI2SCFGR.d32;
                STM32_RCC_REGS.CR.b.PLLI2SON &= on;
                break;
            case PLLSAI:
                PLLCFGR.d32 = STM32_RCC_REGS.PLLSAICFGR.d32;
                STM32_RCC_REGS.CR.b.PLLSAION &= on;
                break;
        }
        PLLCFGR.PLLN = n;
        PLLCFGR.PLLP = (p >> 1) - 1;
        PLLCFGR.PLLQ = q;
        PLLCFGR.PLLR = r;
        switch (pll)
        {
            case MainPLL:
                STM32_RCC_REGS.PLLCFGR.d32 = PLLCFGR.d32;
                STM32_RCC_REGS.CR.b.PLLON |= on;
                break;
            case PLLI2S:
                STM32_RCC_REGS.PLLI2SCFGR.d32 = PLLCFGR.d32;
                STM32_RCC_REGS.CR.b.PLLI2SON |= on;
                break;
            case PLLSAI:
                STM32_RCC_REGS.PLLSAICFGR.d32 = PLLCFGR.d32;
                STM32_RCC_REGS.CR.b.PLLSAION |= on;
                break;
        }
        leave_critical_section();
    }

    void STM32_RCC_OPTIMIZE RCC::configureOscillator(Oscillator osc, bool on, bool bypass)
    {
        enter_critical_section();
        switch (osc)
        {
            case HSI:
                STM32_RCC_REGS.CR.b.HSION = on;
                break;
            case HSE:
                STM32_RCC_REGS.CR.b.HSEON = false;
                STM32_RCC_REGS.CR.b.HSEBYP = bypass;
                STM32_RCC_REGS.CR.b.HSEON = on;
                break;
            case LSI:
                STM32_RCC_REGS.CSR.b.LSION = on;
                break;
            case LSE:
                STM32_RCC_REGS.BDCR.b.LSEON = false;
                STM32_RCC_REGS.BDCR.b.LSEBYP = bypass;
                STM32_RCC_REGS.BDCR.b.LSEON = on;
                break;
        }
        leave_critical_section();
    }
#endif

}

int STM32_RCC_OPTIMIZE cortexm_get_fclk_frequency()
{
    return STM32::RCC::getAHBClockFrequency();
}

int STM32_RCC_OPTIMIZE cortexm_get_systick_frequency()
{
    return STM32::RCC::getAHBClockFrequency() >> 3;
}

void STM32_RCC_OPTIMIZE clockgate_enable(int gate, bool on)
{
    BB_PERIPH(STM32_RCC_REGS.CLKGATES, gate) = on;
}

bool STM32_RCC_OPTIMIZE clockgate_enable_getold(int gate, bool on)
{
    bool lockout = get_critsec_state();
    enter_critical_section();
    bool old = BB_PERIPH(STM32_RCC_REGS.CLKGATES, gate);
    BB_PERIPH(STM32_RCC_REGS.CLKGATES, gate) = on;
    if (!lockout) leave_critical_section();
    return old;
}

bool STM32_RCC_OPTIMIZE resetline_assert(int line, bool on)
{
    return clockgate_enable_getold(((((uintptr_t)&STM32_RCC_REGS.RSTLINES)
                                 - ((uintptr_t)&STM32_RCC_REGS.CLKGATES)) << 3) + line, on);
}
