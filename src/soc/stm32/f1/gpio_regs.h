#pragma once

#include "global.h"

namespace STM32
{

    namespace GPIO_REGS
    {
    
        enum MODE
        {
            MODE_INPUT = 0,
            MODE_10MHZ = 1,
            MODE_2MHZ = 2,
            MODE_50MHZ = 3,
        };

        enum CNF
        {
            CNF_IN_ANALOG = 0,
            CNF_IN_FLOATING = 1,
            CNF_IN_PULL = 2,
            CNF_OUT_PUSHPULL = 0,
            CNF_OUT_OPENDRAIN = 1,
            CNF_AF_PUSHPULL = 2,
            CNF_AF_OPENDRAIN = 3,
        };

        struct __attribute__((packed)) PORT_REGS
        {
            union CR
            {
                uint32_t d32[2];
                struct __attribute__((packed))
                {
                    enum MODE MODE0 : 2;
                    enum CNF CNF0 : 2;
                    enum MODE MODE1 : 2;
                    enum CNF CNF1 : 2;
                    enum MODE MODE2 : 2;
                    enum CNF CNF2 : 2;
                    enum MODE MODE3 : 2;
                    enum CNF CNF3 : 2;
                    enum MODE MODE4 : 2;
                    enum CNF CNF4 : 2;
                    enum MODE MODE5 : 2;
                    enum CNF CNF5 : 2;
                    enum MODE MODE6 : 2;
                    enum CNF CNF6 : 2;
                    enum MODE MODE7 : 2;
                    enum CNF CNF7 : 2;
                    enum MODE MODE8 : 2;
                    enum CNF CNF8 : 2;
                    enum MODE MODE9 : 2;
                    enum CNF CNF9 : 2;
                    enum MODE MODE10 : 2;
                    enum CNF CNF10 : 2;
                    enum MODE MODE11 : 2;
                    enum CNF CNF11 : 2;
                    enum MODE MODE12 : 2;
                    enum CNF CNF12 : 2;
                    enum MODE MODE13 : 2;
                    enum CNF CNF13 : 2;
                    enum MODE MODE14 : 2;
                    enum CNF CNF14 : 2;
                    enum MODE MODE15 : 2;
                    enum CNF CNF15 : 2;
                };
            } CR;
            union IDR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    bool P0 : 1;
                    bool P1 : 1;
                    bool P2 : 1;
                    bool P3 : 1;
                    bool P4 : 1;
                    bool P5 : 1;
                    bool P6 : 1;
                    bool P7 : 1;
                    bool P8 : 1;
                    bool P9 : 1;
                    bool P10 : 1;
                    bool P11 : 1;
                    bool P12 : 1;
                    bool P13 : 1;
                    bool P14 : 1;
                    bool P15 : 1;
                };
            } IDR;
            union ODR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    bool P0 : 1;
                    bool P1 : 1;
                    bool P2 : 1;
                    bool P3 : 1;
                    bool P4 : 1;
                    bool P5 : 1;
                    bool P6 : 1;
                    bool P7 : 1;
                    bool P8 : 1;
                    bool P9 : 1;
                    bool P10 : 1;
                    bool P11 : 1;
                    bool P12 : 1;
                    bool P13 : 1;
                    bool P14 : 1;
                    bool P15 : 1;
                };
            } ODR;
            union BSRR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    union
                    {
                        uint16_t d16;
                        struct __attribute__((packed))
                        {
                            bool P0 : 1;
                            bool P1 : 1;
                            bool P2 : 1;
                            bool P3 : 1;
                            bool P4 : 1;
                            bool P5 : 1;
                            bool P6 : 1;
                            bool P7 : 1;
                            bool P8 : 1;
                            bool P9 : 1;
                            bool P10 : 1;
                            bool P11 : 1;
                            bool P12 : 1;
                            bool P13 : 1;
                            bool P14 : 1;
                            bool P15 : 1;
                        };
                    } set;
                    union
                    {
                        uint16_t d16;
                        struct __attribute__((packed))
                        {
                            bool P0 : 1;
                            bool P1 : 1;
                            bool P2 : 1;
                            bool P3 : 1;
                            bool P4 : 1;
                            bool P5 : 1;
                            bool P6 : 1;
                            bool P7 : 1;
                            bool P8 : 1;
                            bool P9 : 1;
                            bool P10 : 1;
                            bool P11 : 1;
                            bool P12 : 1;
                            bool P13 : 1;
                            bool P14 : 1;
                            bool P15 : 1;
                        };
                    } reset;
                };
            } BSRR;
            union BRR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    bool P0 : 1;
                    bool P1 : 1;
                    bool P2 : 1;
                    bool P3 : 1;
                    bool P4 : 1;
                    bool P5 : 1;
                    bool P6 : 1;
                    bool P7 : 1;
                    bool P8 : 1;
                    bool P9 : 1;
                    bool P10 : 1;
                    bool P11 : 1;
                    bool P12 : 1;
                    bool P13 : 1;
                    bool P14 : 1;
                    bool P15 : 1;
                };
            } BRR;
            union LCKR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    bool P0 : 1;
                    bool P1 : 1;
                    bool P2 : 1;
                    bool P3 : 1;
                    bool P4 : 1;
                    bool P5 : 1;
                    bool P6 : 1;
                    bool P7 : 1;
                    bool P8 : 1;
                    bool P9 : 1;
                    bool P10 : 1;
                    bool P11 : 1;
                    bool P12 : 1;
                    bool P13 : 1;
                    bool P14 : 1;
                    bool P15 : 1;
                    bool K : 1;
                };
            } LCKR;
        };
        
    }
    
}

#define STM32_GPIO_REGS(x) (*((volatile STM32::GPIO_REGS::PORT_REGS*)(0x40010800 + (x) * 0x400)))
#define STM32_GPIOA_REGS STM32_GPIO_REGS(0)
#define STM32_GPIOB_REGS STM32_GPIO_REGS(1)
#define STM32_GPIOC_REGS STM32_GPIO_REGS(2)
#define STM32_GPIOD_REGS STM32_GPIO_REGS(3)
#define STM32_GPIOE_REGS STM32_GPIO_REGS(4)
#define STM32_GPIOF_REGS STM32_GPIO_REGS(5)
#define STM32_GPIOG_REGS STM32_GPIO_REGS(6)
#define STM32_GPIOH_REGS STM32_GPIO_REGS(7)

#define GPIO_CONFIG_PORT( \
    mode0, type0, speed0, pull0, initval0, \
    mode1, type1, speed1, pull1, initval1, \
    mode2, type2, speed2, pull2, initval2, \
    mode3, type3, speed3, pull3, initval3, \
    mode4, type4, speed4, pull4, initval4, \
    mode5, type5, speed5, pull5, initval5, \
    mode6, type6, speed6, pull6, initval6, \
    mode7, type7, speed7, pull7, initval7, \
    mode8, type8, speed8, pull8, initval8, \
    mode9, type9, speed9, pull9, initval9, \
    mode10, type10, speed10, pull10, initval10, \
    mode11, type11, speed11, pull11, initval11, \
    mode12, type12, speed12, pull12, initval12, \
    mode13, type13, speed13, pull13, initval13, \
    mode14, type14, speed14, pull14, initval14, \
    mode15, type15, speed15, pull15, initval15, \
    dummy \
) \
    { initval0, pull0, type0, mode0, speed0, }, \
    { initval1, pull1, type1, mode1, speed1, }, \
    { initval2, pull2, type2, mode2, speed2, }, \
    { initval3, pull3, type3, mode3, speed3, }, \
    { initval4, pull4, type4, mode4, speed4, }, \
    { initval5, pull5, type5, mode5, speed5, }, \
    { initval6, pull6, type6, mode6, speed6, }, \
    { initval7, pull7, type7, mode7, speed7, }, \
    { initval8, pull8, type8, mode8, speed8, }, \
    { initval9, pull9, type9, mode9, speed9, }, \
    { initval10, pull10, type10, mode10, speed10, }, \
    { initval11, pull11, type11, mode11, speed11, }, \
    { initval12, pull12, type12, mode12, speed12, }, \
    { initval13, pull13, type13, mode13, speed13, }, \
    { initval14, pull14, type14, mode14, speed14, }, \
    { initval15, pull15, type15, mode15, speed15, }
