#pragma once

struct __attribute__((packed)) STM32_RCC_REG_TYPE
{
    union __attribute__((packed)) CR
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
            uint32_t PLL2ON : 1;
            uint32_t PLL2RDY : 1;
            uint32_t PLL3ON : 1;
            uint32_t PLL3RDY : 1;
            uint32_t : 2;
        } b;
    } CR;
    union __attribute__((packed)) CFGR
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
                PPRE1_DIV1 = 0x0,
                PPRE1_DIV2 = 0x4,
                PPRE1_DIV4 = 0x5,
                PPRE1_DIV8 = 0x6,
                PPRE1_DIV16 = 0x7,
            } PPRE1 : 3;
            enum
            {
                PPRE2_DIV1 = 0x0,
                PPRE2_DIV2 = 0x4,
                PPRE2_DIV4 = 0x5,
                PPRE2_DIV8 = 0x6,
                PPRE2_DIV16 = 0x7,
            } PPRE2 : 3;
            enum
            {
                ADCPRE_DIV2 = 0x0,
                ADCPRE_DIV4 = 0x1,
                ADCPRE_DIV6 = 0x2,
                ADCPRE_DIV8 = 0x3,
            } ADCPRE : 2;
            enum
            {
                PLLSRC_HSI_DIV2 = 0x0,
                PLLSRC_HSE_PREDIV = 0x1,
            } PLLSRC : 1;
            uint32_t PLLXTPRE : 1;
            enum
            {
                PLLMUL_MUL4 = 0x2,
                PLLMUL_MUL5 = 0x3,
                PLLMUL_MUL6 = 0x4,
                PLLMUL_MUL7 = 0x5,
                PLLMUL_MUL8 = 0x6,
                PLLMUL_MUL9 = 0x7,
                PLLMUL_MUL6_5 = 0xd,
            } PLLMUL : 4;
            enum
            {
                OTGFSPRE_DIV3 = 0x0,
                OTGFSPRE_DIV2 = 0x1,
            } OTGFSPRE : 1;
            uint32_t : 1;
            enum
            {
                MCO_OFF = 0x0,
                MCO_SYSCLK = 0x4,
                MCO_HSI = 0x5,
                MCO_HSE = 0x6,
                MCO_PLLDIV2 = 0x7,
                MCO_PLL2 = 0x8,
                MCO_PLL3DIV2 = 0x9,
                MCO_XT1 = 0xa,
                MCO_PLL3 = 0xb,
            } MCO : 4;
            uint32_t : 4;
        } b;
    } CFGR;
    union __attribute__((packed)) CIR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t LSIRDYF : 1;
            uint32_t LSERDYF : 1;
            uint32_t HSIRDYF : 1;
            uint32_t HSERDYF : 1;
            uint32_t PLLRDYF : 1;
            uint32_t PLL2RDYF : 1;
            uint32_t PLL3RDYF : 1;
            uint32_t CSSF : 1;
            uint32_t LSIRDYIE : 1;
            uint32_t LSERDYIE : 1;
            uint32_t HSIRDYIE : 1;
            uint32_t HSERDYIE : 1;
            uint32_t PLLRDYIE : 1;
            uint32_t PLL2RDYIE : 1;
            uint32_t PLL3RDYIE : 1;
            uint32_t : 1;
            uint32_t LSIRDYC : 1;
            uint32_t LSERDYC : 1;
            uint32_t HSIRDYC : 1;
            uint32_t HSERDYC : 1;
            uint32_t PLLRDYC : 1;
            uint32_t PLL2RDYC : 1;
            uint32_t PLL3RDYC : 1;
            uint32_t CSSC : 1;
            uint32_t : 8;
        } b;
    } CIR;
    union __attribute__((packed)) RSTLINES
    {
        uint32_t d32[2];
        struct __attribute__((packed)) REG
        {
            union __attribute__((packed)) APB2RSTR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t AFIORST : 1;
                    uint32_t : 1;
                    uint32_t IOPARST : 1;
                    uint32_t IOPBRST : 1;
                    uint32_t IOPCRST : 1;
                    uint32_t IOPDRST : 1;
                    uint32_t IOPERST : 1;
                    uint32_t : 2;
                    uint32_t ADC1RST : 1;
                    uint32_t ADC2RST : 1;
                    uint32_t TIM1RST : 1;
                    uint32_t SPI1RST : 1;
                    uint32_t : 1;
                    uint32_t USART1RST : 1;
                    uint32_t : 17;
                } b;
            } APB2RSTR;
            union __attribute__((packed)) APB1RSTR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t TIM2RST : 1;
                    uint32_t TIM3RST : 1;
                    uint32_t TIM4RST : 1;
                    uint32_t TIM5RST : 1;
                    uint32_t TIM6RST : 1;
                    uint32_t TIM7RST : 1;
                    uint32_t : 5;
                    uint32_t WWDGRST : 1;
                    uint32_t : 2;
                    uint32_t SPI2RST : 1;
                    uint32_t SPI3RST : 1;
                    uint32_t : 1;
                    uint32_t USART2RST : 1;
                    uint32_t USART3RST : 1;
                    uint32_t UART4RST : 1;
                    uint32_t UART5RST : 1;
                    uint32_t I2C1RST : 1;
                    uint32_t I2C2RST : 1;
                    uint32_t : 2;
                    uint32_t CAN1RST : 1;
                    uint32_t CAN2RST : 1;
                    uint32_t BKPRST : 1;
                    uint32_t PWRRST : 1;
                    uint32_t DACRST : 1;
                    uint32_t : 2;
                } b;
            } APB1RSTR;
        } r;
    } RSTLINES;
    union __attribute__((packed)) CLKGATES
    {
        uint32_t d32[3];
        struct __attribute__((packed)) REG
        {
            union __attribute__((packed)) AHBENR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t DMA1EN : 1;
                    uint32_t DMA2EN : 1;
                    uint32_t SRAMEN : 1;
                    uint32_t : 1;
                    uint32_t FLITFEN : 1;
                    uint32_t : 1;
                    uint32_t CRCEN : 1;
                    uint32_t : 5;
                    uint32_t OTGFSEN : 1;
                    uint32_t : 1;
                    uint32_t ETHMACEN : 1;
                    uint32_t ETHMACTXEN : 1;
                    uint32_t ETHMACRXEN : 1;
                    uint32_t : 15;
                } b;
            } AHBENR;
            union __attribute__((packed)) APB2ENR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t AFIOEN : 1;
                    uint32_t : 1;
                    uint32_t IOPAEN : 2;
                    uint32_t IOPBEN : 1;
                    uint32_t IOPCEN : 1;
                    uint32_t IOPDEN : 1;
                    uint32_t IOPEEN : 1;
                    uint32_t : 2;
                    uint32_t ADC1EN : 1;
                    uint32_t ADC2EN : 1;
                    uint32_t TIM1EN : 1;
                    uint32_t SPI1EN : 1;
                    uint32_t : 1;
                    uint32_t USART1EN : 1;
                    uint32_t : 17;
                } b;
            } APB2ENR;
            union __attribute__((packed)) APB1ENR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t TIM2EN : 1;
                    uint32_t TIM3EN : 1;
                    uint32_t TIM4EN : 1;
                    uint32_t TIM5EN : 1;
                    uint32_t TIM6EN : 1;
                    uint32_t TIM7EN : 1;
                    uint32_t : 5;
                    uint32_t WWDGEN : 1;
                    uint32_t : 2;
                    uint32_t SPI2EN : 1;
                    uint32_t SPI3EN : 1;
                    uint32_t : 1;
                    uint32_t USART2EN : 1;
                    uint32_t USART3EN : 1;
                    uint32_t UART4EN : 1;
                    uint32_t UART5EN : 1;
                    uint32_t I2C1EN : 1;
                    uint32_t I2C2EN : 1;
                    uint32_t : 2;
                    uint32_t CAN1EN : 1;
                    uint32_t CAN2EN : 1;
                    uint32_t BKPEN : 1;
                    uint32_t PWREN : 1;
                    uint32_t DACEN : 1;
                    uint32_t : 2;
                } b;
            } APB1ENR;
        } r;
    } CLKGATES;
    union __attribute__((packed)) BDCR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t LSEON : 1;
            uint32_t LSERDY : 1;
            uint32_t LSEBYP : 1;
            uint32_t : 5;
            uint32_t RTCSEL : 2;
            uint32_t : 5;
            uint32_t RTCEN : 1;
            uint32_t BDRST : 1;
            uint32_t : 15;
        } b;
    } BDCR;
    union __attribute__((packed)) CSR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t LSION : 1;
            uint32_t LSIRDY : 1;
            uint32_t : 22;
            uint32_t RMVF : 1;
            uint32_t : 1;
            uint32_t PINRSTF : 1;
            uint32_t PORRSTF : 1;
            uint32_t SFTRSTF : 1;
            uint32_t IWDGRSTF : 1;
            uint32_t WWDGRSTF : 1;
            uint32_t LPWRRSTF : 1;
        } b;
    } CSR;
    union __attribute__((packed)) AHBRSTR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t : 12;
            uint32_t OTGFSRST : 1;
            uint32_t : 1;
            uint32_t ETHMACRST : 1;
            uint32_t : 17;
        } b;
    } AHBRSTR;
    union __attribute__((packed)) CFGR2
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t PREDIV1 : 4;
            uint32_t PREDIV2 : 4;
            enum
            {
                PLL2MUL_MUL8 = 0x6,
                PLL2MUL_MUL9 = 0x7,
                PLL2MUL_MUL10 = 0x8,
                PLL2MUL_MUL11 = 0x9,
                PLL2MUL_MUL12 = 0xa,
                PLL2MUL_MUL13 = 0xb,
                PLL2MUL_MUL14 = 0xc,
                PLL2MUL_MUL16 = 0xe,
                PLL2MUL_MUL20 = 0xf,
            } PLL2MUL : 4;
            enum
            {
                PLL3MUL_MUL8 = 0x6,
                PLL3MUL_MUL9 = 0x7,
                PLL3MUL_MUL10 = 0x8,
                PLL3MUL_MUL11 = 0x9,
                PLL3MUL_MUL12 = 0xa,
                PLL3MUL_MUL13 = 0xb,
                PLL3MUL_MUL14 = 0xc,
                PLL3MUL_MUL16 = 0xe,
                PLL3MUL_MUL20 = 0xf,
            } PLL3MUL : 4;
            enum
            {
                PREDIV1SRC_HSE = 0x0,
                PREDIV1SRC_PLL2 = 0x1,
            } PREDIV1SRC : 1;
            enum
            {
                I2S2SRC_SYSCLK = 0x0,
                I2S2SRC_PLL3 = 0x1,
            } I2S2SRC : 1;
            enum
            {
                I2S3SRC_SYSCLK = 0x0,
                I2S3SRC_PLL3 = 0x1,
            } I2S3SRC : 1;
            uint32_t : 13;
        } b;
    } CFGR2;
};

#define STM32_RCC_REGS (*((volatile STM32_RCC_REG_TYPE*)0x40021000))
