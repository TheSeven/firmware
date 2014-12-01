#pragma once

#include "global.h"

namespace STM32
{

    namespace GPIO_REGS
    {
    
        enum MODE
        {
            MODE_INPUT = 0,
            MODE_OUTPUT = 1,
            MODE_SPECIAL = 2,
            MODE_ANALOG = 3,
        };

        enum TYPE
        {
            TYPE_PUSHPULL = 0,
            TYPE_OPENDRAIN = 1,
        };

        enum SPEED
        {
            SPEED_2MHZ = 0,
            SPEED_25MHZ = 1,
            SPEED_50MHZ = 2,
            SPEED_100MHZ = 3,
        };

        enum PULL
        {
            PULL_NONE = 0,
            PULL_UP = 1,
            PULL_DOWN = 2,
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

        enum SPECIAL
        {
#include STM32_GPIO_DEF_FILE
        };

        struct __attribute__((packed)) PORT_REGS
        {
            union MODER
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    enum MODE P0 : 2;
                    enum MODE P1 : 2;
                    enum MODE P2 : 2;
                    enum MODE P3 : 2;
                    enum MODE P4 : 2;
                    enum MODE P5 : 2;
                    enum MODE P6 : 2;
                    enum MODE P7 : 2;
                    enum MODE P8 : 2;
                    enum MODE P9 : 2;
                    enum MODE P10 : 2;
                    enum MODE P11 : 2;
                    enum MODE P12 : 2;
                    enum MODE P13 : 2;
                    enum MODE P14 : 2;
                    enum MODE P15 : 2;
                };
            } MODER;
            union OTYPER
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    enum TYPE P0 : 1;
                    enum TYPE P1 : 1;
                    enum TYPE P2 : 1;
                    enum TYPE P3 : 1;
                    enum TYPE P4 : 1;
                    enum TYPE P5 : 1;
                    enum TYPE P6 : 1;
                    enum TYPE P7 : 1;
                    enum TYPE P8 : 1;
                    enum TYPE P9 : 1;
                    enum TYPE P10 : 1;
                    enum TYPE P11 : 1;
                    enum TYPE P12 : 1;
                    enum TYPE P13 : 1;
                    enum TYPE P14 : 1;
                    enum TYPE P15 : 1;
                };
            } OTYPER;
            union OSPEEDR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    enum SPEED P0 : 2;
                    enum SPEED P1 : 2;
                    enum SPEED P2 : 2;
                    enum SPEED P3 : 2;
                    enum SPEED P4 : 2;
                    enum SPEED P5 : 2;
                    enum SPEED P6 : 2;
                    enum SPEED P7 : 2;
                    enum SPEED P8 : 2;
                    enum SPEED P9 : 2;
                    enum SPEED P10 : 2;
                    enum SPEED P11 : 2;
                    enum SPEED P12 : 2;
                    enum SPEED P13 : 2;
                    enum SPEED P14 : 2;
                    enum SPEED P15 : 2;
                };
            } OSPEEDR;
            union PUPDR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    enum PULL P0 : 2;
                    enum PULL P1 : 2;
                    enum PULL P2 : 2;
                    enum PULL P3 : 2;
                    enum PULL P4 : 2;
                    enum PULL P5 : 2;
                    enum PULL P6 : 2;
                    enum PULL P7 : 2;
                    enum PULL P8 : 2;
                    enum PULL P9 : 2;
                    enum PULL P10 : 2;
                    enum PULL P11 : 2;
                    enum PULL P12 : 2;
                    enum PULL P13 : 2;
                    enum PULL P14 : 2;
                    enum PULL P15 : 2;
                };
            } PUPDR;
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
            union AFR
            {
                uint64_t d64;
                uint32_t d32[2];
                struct __attribute__((packed))
                {
                    enum SPECIAL P0 : 4;
                    enum SPECIAL P1 : 4;
                    enum SPECIAL P2 : 4;
                    enum SPECIAL P3 : 4;
                    enum SPECIAL P4 : 4;
                    enum SPECIAL P5 : 4;
                    enum SPECIAL P6 : 4;
                    enum SPECIAL P7 : 4;
                    enum SPECIAL P8 : 4;
                    enum SPECIAL P9 : 4;
                    enum SPECIAL P10 : 4;
                    enum SPECIAL P11 : 4;
                    enum SPECIAL P12 : 4;
                    enum SPECIAL P13 : 4;
                    enum SPECIAL P14 : 4;
                    enum SPECIAL P15 : 4;
                };
            } AFR;
        };
        
    }
    
}

#ifdef SOC_STM32F0
#define STM32_GPIO_REGS(x) (*((volatile STM32::GPIO_REGS::PORT_REGS*)(0x48000000 + (x) * 0x400)))
#else
#define STM32_GPIO_REGS(x) (*((volatile STM32::GPIO_REGS::PORT_REGS*)(0x40020000 + (x) * 0x400)))
#endif
#define STM32_GPIOA_REGS STM32_GPIO_REGS(0)
#define STM32_GPIOB_REGS STM32_GPIO_REGS(1)
#define STM32_GPIOC_REGS STM32_GPIO_REGS(2)
#define STM32_GPIOD_REGS STM32_GPIO_REGS(3)
#define STM32_GPIOE_REGS STM32_GPIO_REGS(4)
#define STM32_GPIOF_REGS STM32_GPIO_REGS(5)
#define STM32_GPIOG_REGS STM32_GPIO_REGS(6)
#define STM32_GPIOH_REGS STM32_GPIO_REGS(7)
#define STM32_GPIOI_REGS STM32_GPIO_REGS(8)
#define STM32_GPIOJ_REGS STM32_GPIO_REGS(9)
#define STM32_GPIOK_REGS STM32_GPIO_REGS(10)

#define GPIO_CONFIG_PORT( \
    mode0, type0, speed0, pull0, initctl0, initval0, special0, \
    mode1, type1, speed1, pull1, initctl1, initval1, special1, \
    mode2, type2, speed2, pull2, initctl2, initval2, special2, \
    mode3, type3, speed3, pull3, initctl3, initval3, special3, \
    mode4, type4, speed4, pull4, initctl4, initval4, special4, \
    mode5, type5, speed5, pull5, initctl5, initval5, special5, \
    mode6, type6, speed6, pull6, initctl6, initval6, special6, \
    mode7, type7, speed7, pull7, initctl7, initval7, special7, \
    mode8, type8, speed8, pull8, initctl8, initval8, special8, \
    mode9, type9, speed9, pull9, initctl9, initval9, special9, \
    mode10, type10, speed10, pull10, initctl10, initval10, special10, \
    mode11, type11, speed11, pull11, initctl11, initval11, special11, \
    mode12, type12, speed12, pull12, initctl12, initval12, special12, \
    mode13, type13, speed13, pull13, initctl13, initval13, special13, \
    mode14, type14, speed14, pull14, initctl14, initval14, special14, \
    mode15, type15, speed15, pull15, initctl15, initval15, special15, \
    dummy \
) \
{ \
    (uint32_t)(((mode15) << 30) | ((mode14) << 28) | ((mode13) << 26) | ((mode12) << 24) | \
               ((mode11) << 22) | ((mode10) << 20) | ((mode9) << 18) | ((mode8) << 16) | \
               ((mode7) << 14) | ((mode6) << 12) | ((mode5) << 10) | ((mode4) << 8) | \
               ((mode3) << 6) | ((mode2) << 4) | ((mode1) << 2) | (mode0)), \
    (uint32_t)(((type15) << 15) | ((type14) << 14) | ((type13) << 13) | ((type12) << 12) | \
               ((type11) << 11) | ((type10) << 10) | ((type9) << 9) | ((type8) << 8) | \
               ((type7) << 7) | ((type6) << 6) | ((type5) << 5) | ((type4) << 4) | \
               ((type3) << 3) | ((type2) << 2) | ((type1) << 1) | (type0)), \
    (uint32_t)(((speed15) << 30) | ((speed14) << 28) | ((speed13) << 26) | ((speed12) << 24) | \
               ((speed11) << 22) | ((speed10) << 20) | ((speed9) << 18) | ((speed8) << 16) | \
               ((speed7) << 14) | ((speed6) << 12) | ((speed5) << 10) | ((speed4) << 8) | \
               ((speed3) << 6) | ((speed2) << 4) | ((speed1) << 2) | (speed0)), \
    (uint32_t)(((pull15) << 30) | ((pull14) << 28) | ((pull13) << 26) | ((pull12) << 24) | \
               ((pull11) << 22) | ((pull10) << 20) | ((pull9) << 18) | ((pull8) << 16) | \
               ((pull7) << 14) | ((pull6) << 12) | ((pull5) << 10) | ((pull4) << 8) | \
               ((pull3) << 6) | ((pull2) << 4) | ((pull1) << 2) | (pull0)), \
    (uint32_t)(((initctl15) << 31) | ((initctl14) << 30) | ((initctl13) << 29) | ((initctl12) << 28) | \
               ((initctl11) << 27) | ((initctl10) << 26) | ((initctl9) << 25) | ((initctl8) << 24) | \
               ((initctl7) << 23) | ((initctl6) << 22) | ((initctl5) << 21) | ((initctl4) << 20) | \
               ((initctl3) << 19) | ((initctl2) << 18) | ((initctl1) << 17) | ((initctl0) << 16) | \
               ((initval15) << 15) | ((initval14) << 14) | ((initval13) << 13) | ((initval12) << 12) | \
               ((initval11) << 11) | ((initval10) << 10) | ((initval9) << 9) | ((initval8) << 8) | \
               ((initval7) << 7) | ((initval6) << 6) | ((initval5) << 5) | ((initval4) << 4) | \
               ((initval3) << 3) | ((initval2) << 2) | ((initval1) << 1) | (initval0)), \
    (uint32_t)(((special15) << 28) | ((special14) << 24) | ((special13) << 20) | ((special12) << 16) | \
               ((special11) << 12) | ((special10) << 8) | ((special9) << 4) | (special8)), \
    (uint32_t)(((special7) << 28) | ((special6) << 24) | ((special5) << 20) | ((special4) << 16) | \
               ((special3) << 12) | ((special2) << 8) | ((special1) << 4) | (special0)), \
}

