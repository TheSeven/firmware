#pragma once

#include "global.h"


#ifndef STM32_RCC_OPTIMIZE
#define STM32_RCC_OPTIMIZE
#endif


namespace STM32
{
    class __attribute__((packed,aligned(4))) RCC final
    {
    public:
        enum APBBus
        {
            APB1 = 0,
            APB2 = 1,
        };

        enum Oscillator
        {
            HSI = 0,
            HSE = 1,
            LSI = 2,
            LSE = 3,
        };
        
        enum PLL
        {
            MainPLL = 0,
            PLL2 = 1,
            PLL3 = 2,
        };

        enum SysClockSource
        {
            SysClockHSI = 0,
            SysClockHSE = 1,
            SysClockPLL = 2,
        };
        
        enum PLLClockSource
        {
            PLLClockHSI = 0,
            PLLClockHSE = 1,
        };
        
        static void init();
        static bool setClockGate(int gate, bool on);
        static int getSysClockFrequency();
        static int getAHBClockFrequency();
        static int getAPBClockFrequency(APBBus bus);
        static int getADCClockFrequency();
        static int get48MClockFrequency();
#ifdef STM32_RCC_DYNAMIC
        static void setSysClockSource(SysClockSource source);
        static void setAHBDivider(int divider);
        static void setAPBDivider(APBBus bus, int divider);
        static void setADCDivider(int divider);
        static void setPLLClockSource(PLLClockSource source, int divider);
        static void configurePLL(bool on, int factor);
        static void configureOscillator(Oscillator osc, bool on, bool bypass);
#endif
    };
}

