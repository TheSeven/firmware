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
        enum Oscillator
        {
            HSI = 0,
            HSE = 1,
            LSI = 2,
            LSE = 3,
            HSI48 = 4,
        };
        
        enum SysClockSource
        {
            SysClockHSI = 0,
            SysClockHSE = 1,
            SysClockPLL = 2,
            SysClockHSI48 = 3,
        };
        
        enum PLLClockSource
        {
            PLLClockHSIDiv2 = 0,
            PLLClockHSI = 1,
            PLLClockHSE = 2,
            PLLClockHSI48 = 3,
        };
        
        static void init();
        static void initClocks();
        static bool setClockGate(int gate, bool on);
        static int getSysClockFrequency();
        static int getAHBClockFrequency();
        static int getAPBClockFrequency();
#ifdef STM32_RCC_DYNAMIC
        static void setSysClockSource(SysClockSource source);
        static void setAHBDivider(int divider);
        static void setAPBDivider(APBBus bus, int divider);
        static void setPLLClockSource(PLLClockSource source, int divider);
        static void configurePLL(bool on, int factor);
        static void configureOscillator(Oscillator osc, bool on, bool bypass);
#endif
    };
}

