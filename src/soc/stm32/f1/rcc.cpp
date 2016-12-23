#include "global.h"
#include "soc/stm32/f1/rcc.h"
#include "soc/stm32/f1/rcc_regs.h"
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
#if (STM32_ADC_CLOCK) > (STM32_MAX_ADC_CLOCK)
    #error STM32 ADC bus frequency is too high!
#endif

#if defined(STM32_CLOCKSOURCE_HSE) && !defined(STM32_CLOCKSOURCE_HSI)
    #if (STM32_HSE_FREQUENCY) < 3000000
        #error STM32 HSE input frequency is too low! Minimum is 3000000Hz.
    #elif (STM32_HSE_FREQUENCY) > 25000000
        #error STM32 HSE input frequency is too high! Maximum is 25000000Hz.
    #endif
    #if (!defined(STM32_HSE_CRYSTAL) && !defined(STM32_HSE_EXTERNAL)) || (defined(STM32_HSE_CRYSTAL) && defined(STM32_HSE_EXTERNAL))
        #error Invalid STM32 HSE oscillator configuration!
    #endif
    #define STM32_PLL_BYPASS CFGR.b.SW_HSE
    #define STM32_PLL_SOURCE CFGR.b.PLLSRC_HSE_PREDIV
    #define STM32_PLL_SOURCE_FREQUENCY STM32_HSE_FREQUENCY
#elif !defined(STM32_CLOCKSOURCE_HSE) && defined(STM32_CLOCKSOURCE_HSI)
    #define STM32_PLL_BYPASS CFGR.b.SW_HSI
    #define STM32_PLL_SOURCE CFGR.b.PLLSRC_HSI_DIV2
    #define STM32_PLL_SOURCE_FREQUENCY ((STM32_HSI_FREQUENCY)/2)
#else
#error Invalid STM32 clock source configuration!
#endif

#ifdef STM32_NO_PLL
    #define STM32_SYS_CLOCK STM32_PLL_SOURCE_FREQUENCY
    #define STM32_SYS_SOURCE STM32_PLL_BYPASS
#else
    #define STM32_SYS_SOURCE CFGR.b.SW_PLL

    #ifdef STM32_CLOCKSOURCE_HSI
        #define STM32_PLLIN_CLOCK 4000000
        #define STM32_PLLIN_DIVIDER 1
    #else
        #ifndef STM32_PLLIN_CLOCK
            #define STM32_PLLIN_CLOCK 8000000
        #endif
        #if (STM32_PLLIN_CLOCK) < 3000000
            #error STM32 PLL input frequency is too low! Minimum is 3000000Hz.
        #elif (STM32_PLLIN_CLOCK) > 12000000
            #error STM32 PLL input frequency is too high! Maximum is 12000000Hz.
        #endif
        #define STM32_PLLIN_DIVIDER ((STM32_PLL_SOURCE_FREQUENCY) / (STM32_PLL_SOURCE_FREQUENCY))
    #endif
    #if (STM32_PLLIN_CLOCK) * (STM32_PLLIN_DIVIDER) != (STM32_PLL_SOURCE_FREQUENCY)
        #error Can not derive the specified PLL input clock from the specified STM32 input clock!
    #endif

    #if (STM32_SYS_CLOCK) < 18000000
        #error STM32 PLL output frequency is too low! Minimum is 18000000Hz.
    #elif (STM32_SYS_CLOCK) > 72000000
        #error STM32 PLL output frequency is too high! Maximum is 72000000Hz.
    #endif
    #if (STM32_PLLIN_CLOCK) * 13 == (STM32_SYS_CLOCK) * 2
        #define STM32_PLL_MULTIPLIER 15
    #else
        #define STM32_PLL_MULTIPLIER ((STM32_SYS_CLOCK) / (STM32_PLLIN_CLOCK))
        #if (STM32_PLLIN_CLOCK) * (STM32_PLL_MULTIPLIER) != (STM32_SYS_CLOCK)
            #error Can not find a PLL multiplication factor for the specified PLL output frequency!
        #endif
        #if (STM32_PLL_MULTIPLIER) < 4
            #error STM32 PLL multiplication factor is too low! Minimum is 4.
        #elif (STM32_PLL_MULTIPLIER) > 9
            #error STM32 PLL multiplication factor is too high! Maximum is 9.
        #endif
    #endif

    #if ((STM32_SYS_CLOCK) + 47999999) / 24000000 < 2
        #define STM32_48M_DIVIDER 2
    #else
        #define STM32_48M_DIVIDER 3
    #endif
    #if 24000000 * (STM32_48M_DIVIDER) != (STM32_SYS_CLOCK)
        #warning Some STM32 peripherals require the PLL output frequency to be a multiple of 24000000Hz!
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

#if (STM32_ADC_CLOCK) * 2 == (STM32_SYS_CLOCK)
    #define STM32_ADC_DIVIDER CFGR.b.ADCPRE_DIV2
#elif (STM32_ADC_CLOCK) * 4 == (STM32_SYS_CLOCK)
    #define STM32_ADC_DIVIDER CFGR.b.ADCPRE_DIV4
#elif (STM32_ADC_CLOCK) * 6 == (STM32_SYS_CLOCK)
    #define STM32_ADC_DIVIDER CFGR.b.ADCPRE_DIV6
#elif (STM32_ADC_CLOCK) * 8 == (STM32_SYS_CLOCK)
    #define STM32_ADC_DIVIDER CFGR.b.ADCPRE_DIV8
#else
    #error Can not derive the ADC clock from the specified SYSCLK output frequency!
#endif

namespace STM32
{

    void STM32_RCC_OPTIMIZE RCC::init()
    {
        // Reset all peripherals
        STM32_RCC_REGS.AHBRSTR.d32 = 0xffffffff;
        STM32_RCC_REGS.AHBRSTR.d32 = 0;
        STM32_RCC_REGS.RSTLINES.r.APB1RSTR.d32 = 0xffffffff;
        STM32_RCC_REGS.RSTLINES.r.APB1RSTR.d32 = 0;
        STM32_RCC_REGS.RSTLINES.r.APB2RSTR.d32 = 0xffffffff;
        STM32_RCC_REGS.RSTLINES.r.APB2RSTR.d32 = 0;
        
        // Disable all RCC interrupts
        STM32_RCC_REGS.CIR.d32 = 0;
        
        // Set up sane clock gating defaults
        union STM32_RCC_REG_TYPE::CLKGATES::REG::AHBENR AHBENR = { 0 };
        AHBENR.b.SRAMEN = true;
        STM32_RCC_REGS.CLKGATES.r.AHBENR.d32 = AHBENR.d32;
        union STM32_RCC_REG_TYPE::CLKGATES::REG::APB1ENR APB1ENR = { 0 };
        STM32_RCC_REGS.CLKGATES.r.APB1ENR.d32 = APB1ENR.d32;
        union STM32_RCC_REG_TYPE::CLKGATES::REG::APB2ENR APB2ENR = { 0 };
        STM32_RCC_REGS.CLKGATES.r.APB2ENR.d32 = APB2ENR.d32;
        
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

        // Configure HSE input divider
        union STM32_RCC_REG_TYPE::CFGR2 CFGR2 = { 0 };
        CFGR2.b.PREDIV1 = STM32_PLLIN_DIVIDER - 1;
        CFGR2.b.PREDIV1SRC = CFGR2.b.PREDIV1SRC_HSE;
        STM32_RCC_REGS.CFGR2.d32 = CFGR2.d32;
        CFGR.d32 = STM32_RCC_REGS.CFGR.d32;
#endif

#ifndef STM32_NO_PLL
        // Configure and start main PLL
        CFGR.b.PLLSRC = STM32_PLL_SOURCE;
        CFGR.b.PLLMUL = (typeof(CFGR.b.PLLMUL))(STM32_PLL_MULTIPLIER - 2);
        CFGR.b.OTGFSPRE = (typeof(CFGR.b.OTGFSPRE))(STM32_48M_DIVIDER == 2);
        STM32_RCC_REGS.CFGR.d32 = CFGR.d32;
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
        CFGR.b.ADCPRE = STM32_ADC_DIVIDER;
        STM32_RCC_REGS.CFGR.d32 = CFGR.d32;

#ifndef STM32_CLOCKSOURCE_HSI
        // Shut down HSI, we don't need it anymore
        while (STM32_RCC_REGS.CFGR.b.SWS == CFGR.b.SWS_HSI);
        STM32_RCC_REGS.CR.b.HSION = false;
#endif
    }
    
    int STM32_RCC_OPTIMIZE RCC::getSysClockFrequency()
    {
#ifdef STM32_RCC_DYNAMIC
        union STM32_RCC_REG_TYPE::CFGR CFGR = { STM32_RCC_REGS.CFGR.d32 };
        int baseclock;
        switch (CFGR.b.SW)
        {
            case STM32_RCC_REGS.CFGR.b.SW_HSI: return STM32_HSI_FREQUENCY;
            case STM32_RCC_REGS.CFGR.b.SW_HSE: return STM32_HSE_FREQUENCY;
            case STM32_RCC_REGS.CFGR.b.SW_PLL:
                switch (CFGR.b.PLLSRC)
                {
                    case CFGR.b.PLLSRC_HSI: baseclock = STM32_HSI_FREQUENCY / 2; break;
                    case CFGR.b.PLLSRC_HSE: baseclock = STM32_HSE_FREQUENCY / (STM32_RCC_REGS.CFGR2.b.PREDIV + 1); break;
                }
                baseclock *= MIN(CFGR.b.PLLMUL + 2, 16)
                return CFGR.b.PLLMUL & 8 ? baseclock >> 1 : baseclock;
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
        return getSysClockFrequency() * 2 / (STM32_RCC_REGS.CFGR.b.OTGFSPRE ? 2 : 3);
#else
        return STM32_SYS_CLOCK / STM32_48M_DIVIDER;
#endif
    }

    int STM32_RCC_OPTIMIZE RCC::getADCClockFrequency()
    {
#ifdef STM32_RCC_DYNAMIC
        return (getSysClockFrequency() / STM32_RCC_REGS.CFGR.b.ADCPRE) >> 1;
#else
        return STM32_ADC_CLOCK;
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
    
    void STM32_RCC_OPTIMIZE RCC::setADCDivider(int divider)
    {
        enter_critical_section();
        STM32_RCC_REGS.CFGR.b.ADCPRE = (divider - 1) >> 1;
        leave_critical_section();
    }

    void STM32_RCC_OPTIMIZE RCC::setPLLClockSource(PLLClockSource source, int divider)
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
    
    void STM32_RCC_OPTIMIZE RCC::configurePLL(bool on, int factor)
    {
        enter_critical_section();
        union STM32_RCC_REG_TYPE::CFGR CFGR = { STM32_RCC_REGS.CFGR.d32 };
        STM32_RCC_REGS.CR.b.PLLON &= on;
        CFGR.PLLMUL = factor - 2;
        STM32_RCC_REGS.CFGR.d32 = CFGR.d32;
        STM32_RCC_REGS.CR.b.PLLON |= on;
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
