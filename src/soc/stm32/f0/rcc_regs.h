#pragma once

struct __attribute__((packed)) STM32_RCC_REG_TYPE
{
    union CR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t HSION : 1;
            uint32_t HSIRDY : 1;
            uint32_t : 1;
            uint32_t HSITRIM : 5;
            uint32_t HSICAL : 8;
            uint32_t HSEON : 1;
            uint32_t HSERDY : 1;
            uint32_t HSEBYP : 1;
            uint32_t CSSON : 1;
            uint32_t : 4;
            uint32_t PLLON : 1;
            uint32_t PLLRDY : 1;
            uint32_t : 6;
        } b;
    } CR;
    union CFGR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            enum
            {
                SW_HSI = 0x0,
                SW_HSE = 0x1,
                SW_PLL = 0x2,
            } SW : 2;
            enum
            {
                SWS_HSI = 0x0,
                SWS_HSE = 0x1,
                SWS_PLL = 0x2,
            } SWS : 2;
            enum
            {
                HPRE_DIV1 = 0x0,
                HPRE_DIV2 = 0x8,
                HPRE_DIV4 = 0x9,
                HPRE_DIV8 = 0xa,
                HPRE_DIV16 = 0xb,
                HPRE_DIV64 = 0xc,
                HPRE_DIV128 = 0xd,
                HPRE_DIV256 = 0xe,
                HPRE_DIV512 = 0xf,
            } HPRE : 4;
            enum
            {
                PPRE_DIV1 = 0x0,
                PPRE_DIV2 = 0x4,
                PPRE_DIV4 = 0x5,
                PPRE_DIV8 = 0x6,
                PPRE_DIV16 = 0x7,
            } PPRE : 3;
            uint32_t : 3;
            enum
            {
                ADCPRE_DIV2 = 0x0,
                ADCPRE_DIV4 = 0x1,
            } ADCPRE : 1;
            uint32_t : 1;
            enum
            {
                PLLSRC_HSI_DIV2 = 0x0,
                PLLSRC_HSE_PREDIV = 0x1,
            } PLLSRC : 1;
            enum
            {
                PLLXTPRE_PREDIV_DIV1 = 0x0,
                PLLXTPRE_PREDIV_DIV2 = 0x1,
            } PLLXTPRE : 1;
            uint32_t PLLMUL : 4;
            uint32_t : 2;
            enum MCO
            {
                MCO_NOCLOCK = 0x0,
                MCO_HSI14 = 0x1,
                MCO_LSI = 0x2,
                MCO_LSE = 0x3,
                MCO_SYSCLK = 0x4,
                MCO_HSI = 0x5,
                MCO_HSE = 0x6,
                MCO_PLL = 0x7,
            } MCO : 4;
            enum MCOPRE
            {
                MCOPRE_DIV1 = 0x0,
                MCOPRE_DIV2 = 0x1,
                MCOPRE_DIV4 = 0x2,
                MCOPRE_DIV8 = 0x3,
                MCOPRE_DIV16 = 0x4,
                MCOPRE_DIV32 = 0x5,
                MCOPRE_DIV64 = 0x6,
                MCOPRE_DIV128 = 0x7,
            } MCOPRE : 3;
            uint32_t PLLNODIV : 1;
        } b;
    } CFGR;
    union CIR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t LSIRDYF : 1;
            uint32_t LSERDYF : 1;
            uint32_t HSIRDYF : 1;
            uint32_t HSERDYF : 1;
            uint32_t PLLRDYF : 1;
            uint32_t HSI14RDYF : 1;
            uint32_t : 1;
            uint32_t CSSF : 1;
            uint32_t LSIRDYIE : 1;
            uint32_t LSERDYIE : 1;
            uint32_t HSIRDYIE : 1;
            uint32_t HSERDYIE : 1;
            uint32_t PLLRDYIE : 1;
            uint32_t HSI14RDYIE : 1;
            uint32_t : 2;
            uint32_t LSIRDYC : 1;
            uint32_t LSERDYC : 1;
            uint32_t HSIRDYC : 1;
            uint32_t HSERDYC : 1;
            uint32_t PLLRDYC : 1;
            uint32_t HSI14RDYC : 1;
            uint32_t : 1;
            uint32_t CSSC : 1;
            uint32_t : 8;
        } b;
    } CIR;
    union RSTLINES
    {
        uint32_t d32[2];
        struct __attribute__((packed)) REG
        {
            union APB2RSTR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t SYSCFGRST : 1;
                    uint32_t : 8;
                    uint32_t ADCRST : 1;
                    uint32_t : 1;
                    uint32_t TIM1RST : 1;
                    uint32_t SPI1RST : 1;
                    uint32_t : 1;
                    uint32_t USART1RST : 1;
                    uint32_t : 1;
                    uint32_t TIM15RST : 1;
                    uint32_t TIM16RST : 1;
                    uint32_t TIM17RST : 1;
                    uint32_t : 3;
                    uint32_t DBGMCURST : 1;
                    uint32_t : 9;
                } b;
            } APB2RSTR;
            union APB1RSTR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t : 1;
                    uint32_t TIM3RST : 1;
                    uint32_t : 2;
                    uint32_t TIM6RST : 1;
                    uint32_t : 3;
                    uint32_t TIM14RST : 1;
                    uint32_t : 2;
                    uint32_t WWDGRST : 1;
                    uint32_t : 2;
                    uint32_t SPI2RST : 1;
                    uint32_t : 2;
                    uint32_t USART2RST : 1;
                    uint32_t : 3;
                    uint32_t I2C1RST : 1;
                    uint32_t I2C2RST : 1;
                    uint32_t : 5;
                    uint32_t PWRRST : 1;
                    uint32_t : 3;
                } b;
            } APB1RSTR;
        } r;
    } RSTLINES;
    union CLKGATES
    {
        uint32_t d32[3];
        struct __attribute__((packed)) REG
        {
            union AHBENR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t DMAEN : 1;
                    uint32_t : 1;
                    uint32_t SRAMEN : 1;
                    uint32_t : 1;
                    uint32_t FLITFEN : 1;
                    uint32_t : 1;
                    uint32_t CRCEN : 1;
                    uint32_t : 10;
                    uint32_t GPIOAEN : 1;
                    uint32_t GPIOBEN : 1;
                    uint32_t GPIOCEN : 1;
                    uint32_t GPIODEN : 1;
                    uint32_t : 1;
                    uint32_t GPIOFEN : 1;
                    uint32_t : 9;
                } b;
            } AHBENR;
            union APB2ENR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t SYSCFGEN : 1;
                    uint32_t : 8;
                    uint32_t ADCEN : 1;
                    uint32_t : 1;
                    uint32_t TIM1EN : 1;
                    uint32_t SPI1EN : 1;
                    uint32_t : 1;
                    uint32_t USART1EN : 1;
                    uint32_t : 1;
                    uint32_t TIM15EN : 1;
                    uint32_t TIM16EN : 1;
                    uint32_t TIM17EN : 1;
                    uint32_t : 3;
                    uint32_t DBGMCUEN : 1;
                    uint32_t : 9;
                } b;
            } APB2ENR;
            union APB1ENR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t : 1;
                    uint32_t TIM3EN: 1;
                    uint32_t : 2;
                    uint32_t TIM6EN : 1;
                    uint32_t : 3;
                    uint32_t TIM14EN : 1;
                    uint32_t : 2;
                    uint32_t WWDGEN : 1;
                    uint32_t : 2;
                    uint32_t SPI2EN : 1;
                    uint32_t : 2;
                    uint32_t USART2EN : 1;
                    uint32_t : 3;
                    uint32_t I2C1EN : 1;
                    uint32_t I2C2EN : 1;
                    uint32_t : 5;
                    uint32_t PWREN : 1;
                    uint32_t : 3;
                } b;
            } APB1ENR;
        } r;
    } CLKGATES;
    union BDCR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t LSEON : 1;
            uint32_t LSERDY : 1;
            uint32_t LSEBYP : 1;
            uint32_t LSEDRV : 2;
            uint32_t : 3;
            enum RTCSEL
            {
                RTCSEL_NOCLOCK = 0x0,
                RTCSEL_LSE = 0x1,
                RTCSEL_HSE = 0x3,
                RTCSEL_LSI = 0x2,
            } RTCSEL : 2;
            uint32_t : 5;
            uint32_t RTCEN : 1;
            uint32_t BDRST : 1;
            uint32_t : 15;
        } b;
    } BDCR;
    union CSR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t LSION : 1;
            uint32_t LSIRDY : 1;
            uint32_t : 21;
            uint32_t V18PWRRSTF : 1;
            uint32_t RMVF : 1;
            uint32_t OBLRSTF : 1;
            uint32_t PINRSTF : 1;
            uint32_t PORRSTF : 1;
            uint32_t SFTRSTF : 1;
            uint32_t IWDGRSTF : 1;
            uint32_t WWDGRSTF : 1;
            uint32_t LPWRRSTF : 1;
        } b;
    } CSR;
    union AHBRSTR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t : 17;
            uint32_t GPIOARST : 1;
            uint32_t GPIOBRST : 1;
            uint32_t GPIOCRST : 1;
            uint32_t GPIODRST : 1;
            uint32_t : 1;
            uint32_t GPIOFRST : 1;
            uint32_t : 9;
        } b;
    } AHBRSTR;
    union CFGR2
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t PREDIV : 4;
            uint32_t : 28;
        } b;
    } CFGR2;
    union CFGR3
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            enum USART1SW
            {
                USART1SW_PCLK = 0x0,
                USART1SW_SYSCLK = 0x1,
                USART1SW_LSE = 0x2,
                USART1SW_HSI = 0x3,
            } USART1SW : 2;
            uint32_t : 2;
            enum I2C1SW
            {
                I2C1SW_HSI = 0x0,
                I2C1SW_SYSCLK = 0x1,
            } I2C1SW : 1;
            uint32_t : 11;
            enum USART2SW
            {
                USART2SW_PCLK = 0x0,
                USART2SW_SYSCLK = 0x1,
                USART2SW_LSE = 0x2,
                USART2SW_HSI = 0x3,
            } USART2SW : 2;
            uint32_t : 14;
        } b;
    } CFGR3;
    union CR2
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t HSI14ON : 1;
            uint32_t HSI14RDY : 1;
            uint32_t HSI14DIS : 1;
            uint32_t HSI14TRIM : 5;
            uint32_t HSI14CAL : 8;
            uint32_t : 16;
        } b;
    } CR2;
};

#define STM32_RCC_REGS (*((volatile STM32_RCC_REG_TYPE*)0x40021000))
