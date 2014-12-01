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

        enum INITCTL
        {
            INITCTL_SET = 0,
            INITCTL_SKIP = 1,
        };

        enum INITVAL
        {
            INITVAL_LOW = 0,
            INITVAL_HIGH = 1,
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
    mode0, cnf0, initctl0, initval0, \
    mode1, cnf1, initctl1, initval1, \
    mode2, cnf2, initctl2, initval2, \
    mode3, cnf3, initctl3, initval3, \
    mode4, cnf4, initctl4, initval4, \
    mode5, cnf5, initctl5, initval5, \
    mode6, cnf6, initctl6, initval6, \
    mode7, cnf7, initctl7, initval7, \
    mode8, cnf8, initctl8, initval8, \
    mode9, cnf9, initctl9, initval9, \
    mode10, cnf10, initctl10, initval10, \
    mode11, cnf11, initctl11, initval11, \
    mode12, cnf12, initctl12, initval12, \
    mode13, cnf13, initctl13, initval13, \
    mode14, cnf14, initctl14, initval14, \
    mode15, cnf15, initctl15, initval15, \
    dummy \
) \
{ \
    (uint32_t)(((cnf7) << 30) | ((mode7) << 28) | ((cnf6) << 26) | ((mode6) << 24) | \
               ((cnf5) << 22) | ((mode5) << 20) | ((cnf4) << 18) | ((mode4) << 16) | \
               ((cnf3) << 14) | ((mode3) << 12) | ((cnf2) << 10) | ((mode2) << 8) | \
               ((cnf1) << 6) | ((mode1) << 4) | ((cnf0) << 2) | (mode0)), \
    (uint32_t)(((cnf15) << 30) | ((mode15) << 28) | ((cnf14) << 26) | ((mode14) << 24) | \
               ((cnf13) << 22) | ((mode13) << 20) | ((cnf12) << 18) | ((mode12) << 16) | \
               ((cnf11) << 14) | ((mode11) << 12) | ((cnf10) << 10) | ((mode10) << 8) | \
               ((cnf9) << 6) | ((mode9) << 4) | ((cnf8) << 2) | (mode8)), \
    (uint32_t)(((initctl15) << 31) | ((initctl14) << 30) | ((initctl13) << 29) | ((initctl12) << 28) | \
               ((initctl11) << 27) | ((initctl10) << 26) | ((initctl9) << 25) | ((initctl8) << 24) | \
               ((initctl7) << 23) | ((initctl6) << 22) | ((initctl5) << 21) | ((initctl4) << 20) | \
               ((initctl3) << 19) | ((initctl2) << 18) | ((initctl1) << 17) | ((initctl0) << 16) | \
               ((initval15) << 15) | ((initval14) << 14) | ((initval13) << 13) | ((initval12) << 12) | \
               ((initval11) << 11) | ((initval10) << 10) | ((initval9) << 9) | ((initval8) << 8) | \
               ((initval7) << 7) | ((initval6) << 6) | ((initval5) << 5) | ((initval4) << 4) | \
               ((initval3) << 3) | ((initval2) << 2) | ((initval1) << 1) | (initval0)), \
}
