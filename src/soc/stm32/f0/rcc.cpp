#include "global.h"
#include "soc/stm32/f0/rcc.h"
#include "soc/stm32/f0/rcc_regs.h"
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
#if (STM32_APB1_CLOCK) != (STM32_APB2_CLOCK)
    #error STM32 APB bus frequencies must match on this device!
#endif

#if defined(STM32_CLOCKSOURCE_HSE) && !defined(STM32_CLOCKSOURCE_HSI) && !defined(STM32_CLOCKSOURCE_HSI48)
    #if (STM32_HSE_FREQUENCY) < 4000000
        #error STM32 HSE input frequency is too low! Minimum is 4000000Hz.
    #elif (STM32_HSE_FREQUENCY) > 32000000
        #error STM32 HSE input frequency is too high! Maximum is 32000000Hz.
    #endif
    #if (!defined(STM32_HSE_CRYSTAL) && !defined(STM32_HSE_EXTERNAL)) || (defined(STM32_HSE_CRYSTAL) && defined(STM32_HSE_EXTERNAL))
        #error Invalid STM32 HSE oscillator configuration!
    #endif
    #define STM32_PLL_BYPASS CFGR.b.SW_HSE
    #define STM32_PLL_SOURCE CFGR.b.PLLSRC_HSE_PREDIV
    #define STM32_PLL_SOURCE_FREQUENCY STM32_HSE_FREQUENCY
#elif !defined(STM32_CLOCKSOURCE_HSE) && defined(STM32_CLOCKSOURCE_HSI) && !defined(STM32_CLOCKSOURCE_HSI48)
    #define STM32_PLL_BYPASS CFGR.b.SW_HSI
    #if defined(SOC_STM32F030) && !defined(SOC_STM32F0XXXC)
        #define STM32_PLL_SOURCE CFGR.b.PLLSRC_HSI_DIV2
        #define STM32_PLL_SOURCE_FREQUENCY ((STM32_HSI_FREQUENCY)/2)
    #else
        #define STM32_PLL_SOURCE CFGR.b.PLLSRC_HSI_PREDIV
        #define STM32_PLL_SOURCE_FREQUENCY STM32_HSI_FREQUENCY
    #endif
#elif !defined(STM32_CLOCKSOURCE_HSE) && !defined(STM32_CLOCKSOURCE_HSI) && defined(STM32_CLOCKSOURCE_HSI48)
    #define STM32_PLL_BYPASS CFGR.b.SW_HSI48
    #define STM32_PLL_SOURCE CFGR.b.PLLSRC_HSI48_PREDIV
    #define STM32_PLL_SOURCE_FREQUENCY STM32_HSI48_FREQUENCY
#else
#error Invalid STM32 clock source configuration!
#endif

#ifdef STM32_NO_PLL
    #ifdef STM32_CLOCKSOURCE_HSE
        #if STM32_SYS_CLOCK != STM32_HSE_FREQUENCY
            #error STM32 system clock does not match external input clock, and PLL is disabled
        #endif
    #endif
    #ifdef STM32_CLOCKSOURCE_HSI
        #if STM32_SYS_CLOCK != STM32_HSI_FREQUENCY
            #error STM32 system clock does not match internal oscillator clock, and PLL is disabled
        #endif
    #endif
    #ifdef STM32_CLOCKSOURCE_HSI48
        #if STM32_SYS_CLOCK != STM32_HSI48_FREQUENCY
            #error STM32 system clock does not match internal oscillator clock, and PLL is disabled
        #endif
    #endif
    #define STM32_SYS_SOURCE STM32_PLL_BYPASS
#else
    #define STM32_SYS_SOURCE CFGR.b.SW_PLL

    #ifndef STM32_PLLIN_CLOCK
        #define STM32_PLLIN_CLOCK 4000000
    #endif
    #if (STM32_PLLIN_CLOCK) < 1000000
        #error STM32 PLL input frequency is too low! Minimum is 1000000Hz.
    #elif (STM32_PLLIN_CLOCK) > 24000000
        #error STM32 PLL input frequency is too high! Maximum is 24000000Hz.
    #endif
    #if defined(STM32_CLOCKSOURCE_HSI) && defined(SOC_STM32F030) && !defined(SOC_STM32F0XXXC)
        #define STM32_PLLIN_DIVIDER 1
    #else
        #define STM32_PLLIN_DIVIDER ((STM32_PLL_SOURCE_FREQUENCY) / (STM32_PLLIN_CLOCK))
    #endif
    #if (STM32_PLLIN_CLOCK) * (STM32_PLLIN_DIVIDER) != (STM32_PLL_SOURCE_FREQUENCY)
        #error Can not derive the specified PLL input clock from the specified STM32 input clock!
    #endif

    #if (STM32_SYS_CLOCK) < 16000000
        #error STM32 PLL output frequency is too low! Minimum is 16000000Hz.
    #elif (STM32_SYS_CLOCK) > 48000000
        #error STM32 PLL output frequency is too high! Maximum is 48000000Hz.
    #endif
    #define STM32_PLL_MULTIPLIER ((STM32_SYS_CLOCK) / (STM32_PLLIN_CLOCK))
    #if (STM32_PLLIN_CLOCK) * (STM32_PLL_MULTIPLIER) != (STM32_SYS_CLOCK)
        #error Can not find a PLL multiplication factor for the specified PLL output frequency!
    #endif
    #if (STM32_PLL_MULTIPLIER) < 2
        #error STM32 PLL multiplication factor is too low! Minimum is 2.
    #elif (STM32_PLL_MULTIPLIER) > 16
        #error STM32 PLL multiplication factor is too high! Maximum is 16.
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
    #define STM32_APB1_DIVIDER CFGR.b.PPRE_DIV1
#elif (STM32_APB1_CLOCK) * 2 == (STM32_AHB_CLOCK)
    #define STM32_APB1_DIVIDER CFGR.b.PPRE_DIV2
#elif (STM32_APB1_CLOCK) * 4 == (STM32_AHB_CLOCK)
    #define STM32_APB1_DIVIDER CFGR.b.PPRE_DIV4
#elif (STM32_APB1_CLOCK) * 8 == (STM32_AHB_CLOCK)
    #define STM32_APB1_DIVIDER CFGR.b.PPRE_DIV8
#elif (STM32_APB1_CLOCK) * 16 == (STM32_AHB_CLOCK)
    #define STM32_APB1_DIVIDER CFGR.b.PPRE_DIV16
#else
    #error Can not derive the APB1 clock from the specified AHB clock frequency!
#endif


namespace STM32
{

    void RCC::init()
    {
        // Reset all peripherals
        STM32_RCC_REGS.RSTLINES.r.APB2RSTR.d32 = 0xffffffff;
        STM32_RCC_REGS.RSTLINES.r.APB2RSTR.d32 = 0;
        STM32_RCC_REGS.RSTLINES.r.APB1RSTR.d32 = 0xffffffff;
        STM32_RCC_REGS.RSTLINES.r.APB1RSTR.d32 = 0;
        
        // Disable all RCC interrupts
        STM32_RCC_REGS.CIR.d32 = 0;
        
        // Set up sane clock gating defaults
        union STM32_RCC_REG_TYPE::CLKGATES::REG::AHBENR AHBENR = { 0 };
        AHBENR.b.SRAMEN = true;
        STM32_RCC_REGS.CLKGATES.r.AHBENR.d32 = AHBENR.d32;
        union STM32_RCC_REG_TYPE::CLKGATES::REG::APB2ENR APB2ENR = { 0 };
        STM32_RCC_REGS.CLKGATES.r.APB2ENR.d32 = APB2ENR.d32;
        union STM32_RCC_REG_TYPE::CLKGATES::REG::APB1ENR APB1ENR = { 0 };
        STM32_RCC_REGS.CLKGATES.r.APB1ENR.d32 = APB1ENR.d32;
        union STM32_RCC_REG_TYPE::CFGR CFGR = { 0 };

        // Start up HSI
        STM32_RCC_REGS.CR.b.HSION = true;
        
        // Shut down HSI14 and HSI48 (or start the latter if we want to use it as our clock source)
        union STM32_RCC_REG_TYPE::CR2 CR2 = { STM32_RCC_REGS.CR2.d32 };
        CR2.b.HSI14ON = false;
#ifdef STM32_CLOCKSOURCE_HSI48
        CR2.b.HSI48ON = true;
#else
        CR2.b.HSI48ON = false;
#endif
        STM32_RCC_REGS.CR2.d32 = CR2.d32;

        // Switch to HSI so that we can shutdown and reconfigure everything else
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

        // Configure HSE input divider
        union STM32_RCC_REG_TYPE::CFGR2 CFGR2 = { 0 };
        CFGR.b.PREDIV = STM32_PLLIN_DIVIDER - 1;
        STM32_RCC_REGS.CFGR2.d32 = CFGR2.d32;
#endif

#ifndef STM32_NO_PLL
        // Configure and start main PLL
        CFGR.b.PLLSRC = STM32_PLL_SOURCE;
        CFGR.b.PLLMUL = STM32_PLL_MULTIPLIER - 2;
        STM32_RCC_REGS.CFGR.d32 = CFGR.d32;
        STM32_RCC_REGS.CR.b.PLLON = true;
#endif

        // Initialize power management while the PLL is locking or HSI48 is starting up
        PWR::init();
        
        // Set flash wait states for higher clock frequency
        FLASH::init();
        
        // Configure system clock source and prescalers
        CFGR.b.SW = STM32_SYS_SOURCE;
        CFGR.b.HPRE = STM32_AHB_DIVIDER;
        CFGR.b.PPRE = STM32_APB1_DIVIDER;
        STM32_RCC_REGS.CFGR.d32 = CFGR.d32;

#ifndef STM32_CLOCKSOURCE_HSI
        // Shut down HSI, we don't need it anymore
        while (STM32_RCC_REGS.CFGR.b.SWS == CFGR.b.SWS_HSI);
        STM32_RCC_REGS.CR.b.HSION = false;
#endif
    }
    
    int RCC::getSysClockFrequency()
    {
#ifdef STM32_RCC_DYNAMIC
        union STM32_RCC_REG_TYPE::CFGR CFGR = { STM32_RCC_REGS.CFGR.d32 };
        int baseclock;
        switch (CFGR.b.SW)
        {
            case STM32_RCC_REGS.CFGR.b.SW_HSI: return STM32_HSI_FREQUENCY;
            case STM32_RCC_REGS.CFGR.b.SW_HSE: return STM32_HSE_FREQUENCY;
            case STM32_RCC_REGS.CFGR.b.SW_HSI48: return STM32_HSI48_FREQUENCY;
            case STM32_RCC_REGS.CFGR.b.SW_PLL:
                switch (CFGR.b.PLLSRC)
                {
                    case CFGR.b.PLLSRC_HSI_DIV2: baseclock = STM32_HSI_FREQUENCY / 2; break;
                    case CFGR.b.PLLSRC_HSI_PREDIV: baseclock = STM32_HSI_FREQUENCY / (STM32_RCC_REGS.CFGR2.b.PREDIV + 1); break;
                    case CFGR.b.PLLSRC_HSE_PREDIV: baseclock = STM32_HSE_FREQUENCY / (STM32_RCC_REGS.CFGR2.b.PREDIV + 1); break;
                    case CFGR.b.PLLSRC_HSI48_PREDIV: baseclock = STM32_HSI48_FREQUENCY / (STM32_RCC_REGS.CFGR2.b.PREDIV + 1); break;
                }
                return baseclock * MIN(CFGR.b.PLLMUL + 2, 16);
            default: return 0;
        }
#else
        return STM32_SYS_CLOCK;
#endif
    }
    
    int RCC::getAHBClockFrequency()
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
    
    int RCC::getAPBClockFrequency()
    {
#ifdef STM32_RCC_DYNAMIC
        int prescaler = STM32_RCC_REGS.CFGR.b.PPRE;
        int baseclock = getAHBClockFrequency();
        if (prescaler & 4) baseclock >>= 1;
        return baseclock >> (prescaler & 3);
#else
        return STM32_APB1_CLOCK;
#endif
    }

#ifdef STM32_RCC_DYNAMIC
    void RCC::setSysClockSource(SysClockSource source)
    {
        enter_critical_section();
        STM32_RCC_REGS.CFGR.b.SW = source;
        while (STM32_RCC_REGS.CFGR.b.SWS != source);
        leave_critical_section();
    }
    
    void RCC::setAHBDivider(int divider)
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
    
    void RCC::setAPBDivider(int divider)
    {
        int value = 0;
        if (divider > 1) value = 4;
        if (divider > 2) value++;
        if (divider > 4) value++;
        if (divider > 8) value++;
        enter_critical_section();
        STM32_RCC_REGS.CFGR.b.PPRE = value;
        leave_critical_section();
    }
    
    void RCC::setPLLClockSource(PLLClockSource source, int divider)
    {
        enter_critical_section();
        union STM32_RCC_REG_TYPE::CFGR CFGR = { STM32_RCC_REGS.CFGR.d32 };
        CFGR.PLLSRC = source;
        STM32_RCC_REGS.CFGR.d32 = CFGR.d32;
        union STM32_RCC_REG_TYPE::CFGR2 CFGR2 = { 0 };
        CFGR2.b.PREDIV = divider - 1;
        STM32_RCC_REGS.CFGR2.d32 = CFGR2.d32;
        leave_critical_section();
    }
    
    void RCC::configurePLL(bool on, int factor)
    {
        enter_critical_section();
        union STM32_RCC_REG_TYPE::CFGR CFGR = { STM32_RCC_REGS.CFGR.d32 };
        STM32_RCC_REGS.CR.b.PLLON &= on;
        CFGR.PLLMUL = factor - 2;
        STM32_RCC_REGS.CFGR.d32 = CFGR.d32;
        STM32_RCC_REGS.CR.b.PLLON |= on;
        leave_critical_section();
    }

    void RCC::configureOscillator(Oscillator osc, bool on, bool bypass)
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
            case HSI48:
                STM32_RCC_REGS.CR2.b.HSION = on;
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

int cortexm_get_fclk_frequency()
{
    return STM32::RCC::getAHBClockFrequency();
}

int cortexm_get_systick_frequency()
{
    return STM32::RCC::getAHBClockFrequency() >> 3;
}

bool __attribute__((optimize("-Os"))) clockgate_enable(int gate, bool on)
{
    volatile uint32_t* reg = &STM32_RCC_REGS.CLKGATES.d32[gate >> 5];
    int bit = gate & 0x1f;
    enter_critical_section();
    uint32_t data = *reg;
    bool old = (data >> bit) & 1;
    data &= ~(1 << bit);
    data |= on << bit;
    *reg = data;
    leave_critical_section();
    return old;
}

bool resetline_assert(int line, bool on)
{
    return clockgate_enable(((((uintptr_t)&STM32_RCC_REGS.RSTLINES)
                            - ((uintptr_t)&STM32_RCC_REGS.CLKGATES)) << 3) + line, on);
}
