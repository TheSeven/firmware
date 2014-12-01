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
            uint32_t PLLI2SON : 1;
            uint32_t PLLI2SRDY : 1;
            uint32_t PLLSAION : 1;
            uint32_t PLLSAIRDY : 1;
            uint32_t : 2;
        } b;
    } CR;
    union PLLCFGR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t PLLM : 6;
            uint32_t PLLN : 9;
            uint32_t : 1;
            uint32_t PLLP : 2;
            uint32_t : 4;
            enum
            {
                PLLSRC_HSI = 0x0,
                PLLSRC_HSE = 0x1,
            } PLLSRC : 1;
            uint32_t : 1;
            uint32_t PLLQ : 4;
            uint32_t PLLR : 3;
            uint32_t : 1;
        } b;
    } PLLCFGR;
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
            uint32_t : 2;
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
            uint32_t RTCPRE : 5;
            uint32_t MCO1 : 2;
            uint32_t I2SSRC : 1;
            uint32_t MCO1PRE : 3;
            uint32_t MCO2PRE : 3;
            uint32_t MCO2 : 2;
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
            uint32_t PLLI2SRDYF : 1;
            uint32_t PLLSAIRDYF : 1;
            uint32_t CSSF : 1;
            uint32_t LSIRDYIE : 1;
            uint32_t LSERDYIE : 1;
            uint32_t HSIRDYIE : 1;
            uint32_t HSERDYIE : 1;
            uint32_t PLLRDYIE : 1;
            uint32_t PLLI2SRDYIE : 1;
            uint32_t PLLSAIRDYIE : 1;
            uint32_t : 1;
            uint32_t LSIRDYC : 1;
            uint32_t LSERDYC : 1;
            uint32_t HSIRDYC : 1;
            uint32_t HSERDYC : 1;
            uint32_t PLLRDYC : 1;
            uint32_t PLLI2SRDYC : 1;
            uint32_t PLLSAIRDYC : 1;
            uint32_t CSSC : 1;
            uint32_t : 8;
        } b;
    } CIR;
    union RSTLINES
    {
        uint32_t d32[8];
        struct __attribute__((packed)) REG
        {
            union AHB1RSTR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t GPIOARST : 1;
                    uint32_t GPIOBRST : 1;
                    uint32_t GPIOCRST : 1;
                    uint32_t GPIODRST : 1;
                    uint32_t GPIOERST : 1;
                    uint32_t GPIOFRST : 1;
                    uint32_t GPIOGRST : 1;
                    uint32_t GPIOHRST : 1;
                    uint32_t GPIOIRST : 1;
                    uint32_t GPIOJRST : 1;
                    uint32_t GPIOKRST : 1;
                    uint32_t : 1;
                    uint32_t CRCRST : 1;
                    uint32_t : 8;
                    uint32_t DMA1RST : 1;
                    uint32_t DMA2RST : 1;
                    uint32_t DMA2DRST : 1;
                    uint32_t : 1;
                    uint32_t ETHMACRST : 1;
                    uint32_t : 2;
                    uint32_t OTGHRST : 1;
                    uint32_t : 3;
                } b;
            } AHB1RSTR;
            union AHB2RSTR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t DCMIRST : 1;
                    uint32_t : 3;
                    uint32_t CRYPRST : 1;
                    uint32_t HASHRST : 1;
                    uint32_t RNGRST : 1;
                    uint32_t OTGFSRST : 1;
                    uint32_t : 24;
                } b;
            } AHB2RSTR;
            union AHB3RSTR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t FMCRST : 1;
                    uint32_t : 31;
                } b;
            } AHB3RSTR;
            uint32_t RESERVED0;
            union APB1RSTR
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
                    uint32_t TIM12RST : 1;
                    uint32_t TIM13RST : 1;
                    uint32_t TIM14RST : 1;
                    uint32_t : 2;
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
                    uint32_t I2C3RST : 1;
                    uint32_t : 1;
                    uint32_t CAN1RST : 1;
                    uint32_t CAN2RST : 1;
                    uint32_t : 1;
                    uint32_t PWRRST : 1;
                    uint32_t DACRST : 1;
                    uint32_t UART7RST : 1;
                    uint32_t UART8RST : 1;
                } b;
            } APB1RSTR;
            union APB2RSTR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t TIM1RST : 1;
                    uint32_t TIM8RST : 1;
                    uint32_t : 2;
                    uint32_t USART1RST : 1;
                    uint32_t USART6RST : 1;
                    uint32_t : 2;
                    uint32_t ADCRST : 1;
                    uint32_t : 2;
                    uint32_t SDIORST : 1;
                    uint32_t SPI1 : 1;
                    uint32_t SPI4RST : 1;
                    uint32_t SYSCFGRST : 1;
                    uint32_t : 1;
                    uint32_t TIM9RST : 1;
                    uint32_t TIM10RST : 1;
                    uint32_t TIM11RST : 1;
                    uint32_t : 1;
                    uint32_t SPI5RST : 1;
                    uint32_t SPI6RST : 1;
                    uint32_t SAI1RST : 1;
                    uint32_t : 3;
                    uint32_t LTDCRST : 1;
                    uint32_t : 5;
                } b;
            } APB2RSTR;
            uint32_t RESERVED1[2];
        } r;
    } RSTLINES;
    union CLKGATES
    {
        uint32_t d32[8];
        struct __attribute__((packed)) REG
        {
            union AHB1ENR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t GPIOAEN : 1;
                    uint32_t GPIOBEN : 1;
                    uint32_t GPIOCEN : 1;
                    uint32_t GPIODEN : 1;
                    uint32_t GPIOEEN : 1;
                    uint32_t GPIOFEN : 1;
                    uint32_t GPIOGEN : 1;
                    uint32_t GPIOHEN : 1;
                    uint32_t GPIOIEN : 1;
                    uint32_t GPIOJEN : 1;
                    uint32_t GPIOKEN : 1;
                    uint32_t : 1;
                    uint32_t CRCEN : 1;
                    uint32_t : 5;
                    uint32_t BKPSRAMEN : 1;
                    uint32_t : 1;
                    uint32_t CCMDATARAMEN : 1;
                    uint32_t DMA1EN : 1;
                    uint32_t DMA2EN : 1;
                    uint32_t DMA2DEN : 1;
                    uint32_t : 1;
                    uint32_t ETHMACEN : 1;
                    uint32_t ETHMACTXEN : 1;
                    uint32_t ETHMACRXEN : 1;
                    uint32_t ETHMACPTPEN : 1;
                    uint32_t OTGHSEN : 1;
                    uint32_t OTGHSULPIEN : 1;
                    uint32_t : 1;
                } b;
            } AHB1ENR;
            union AHB2ENR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t DCMIEN : 1;
                    uint32_t : 3;
                    uint32_t CRYPEN : 1;
                    uint32_t HASHEN : 1;
                    uint32_t RNGEN : 1;
                    uint32_t OTGFSEN : 1;
                    uint32_t : 24;
                } b;
            } AHB2ENR;
            union AHB3ENR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t FMCEN : 1;
                    uint32_t : 31;
                } b;
            } AHB3ENR;
            uint32_t RESERVED2;
            union APB1ENR
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
                    uint32_t TIM12EN : 1;
                    uint32_t TIM13EN : 1;
                    uint32_t TIM14EN : 1;
                    uint32_t : 2;
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
                    uint32_t I2C3EN : 1;
                    uint32_t : 1;
                    uint32_t CAN1EN : 1;
                    uint32_t CAN2EN : 1;
                    uint32_t : 1;
                    uint32_t PWREN : 1;
                    uint32_t DACEN : 1;
                    uint32_t UART7EN : 1;
                    uint32_t UART8EN : 1;
                } b;
            } APB1ENR;
            union APB2ENR
            {
                uint32_t d32;
                struct __attribute__((packed))
                {
                    uint32_t TIM1EN : 1;
                    uint32_t TIM8EN : 1;
                    uint32_t : 2;
                    uint32_t USART1EN : 1;
                    uint32_t USART6EN : 1;
                    uint32_t : 2;
                    uint32_t ADC1EN : 1;
                    uint32_t ADC2EN : 1;
                    uint32_t ADC3EN : 1;
                    uint32_t SDIOEN : 1;
                    uint32_t SPI1EN : 1;
                    uint32_t SPI4EN : 1;
                    uint32_t SYSCFGEN : 1;
                    uint32_t : 1;
                    uint32_t TIM9EN : 1;
                    uint32_t TIM10EN : 1;
                    uint32_t TIM11EN : 1;
                    uint32_t : 1;
                    uint32_t SPI5EN : 1;
                    uint32_t SPI6EN : 1;
                    uint32_t SAI1EN : 1;
                    uint32_t : 3;
                    uint32_t LTDCEN : 1;
                    uint32_t : 5;
                } b;
            } APB2ENR;
            uint32_t RESERVED3[2];
        } r;
    } CLKGATES;
    union AHB1LPENR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t GPIOALPEN : 1;
            uint32_t GPIOBLPEN : 1;
            uint32_t GPIOCLPEN : 1;
            uint32_t GPIODLPEN : 1;
            uint32_t GPIOELPEN : 1;
            uint32_t GPIOFLPEN : 1;
            uint32_t GPIOGLPEN : 1;
            uint32_t GPIOHLPEN : 1;
            uint32_t GPIOILPEN : 1;
            uint32_t GPIOJLPEN : 1;
            uint32_t GPIOKLPEN : 1;
            uint32_t : 1;
            uint32_t CRCLPEN : 1;
            uint32_t : 2;
            uint32_t FLITFLPEN : 1;
            uint32_t SRAM1LPEN : 1;
            uint32_t SRAM2LPEN : 1;
            uint32_t BKPSRAMLPEN : 1;
            uint32_t SRAM3LPEN : 1;
            uint32_t : 1;
            uint32_t DMA1LPEN : 1;
            uint32_t DMA2LPEN : 1;
            uint32_t DMA2DLPEN : 1;
            uint32_t : 1;
            uint32_t ETHMACLPEN : 1;
            uint32_t ETHMACTXLPEN : 1;
            uint32_t ETHMACRXLPEN : 1;
            uint32_t ETHMACPTPLPEN : 1;
            uint32_t OTGHSLPEN : 1;
            uint32_t OTGHSULPILPEN : 1;
            uint32_t : 1;
        } b;
    } AHB1LPENR;
    union AHB2LPENR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t DCMILPEN : 1;
            uint32_t : 3;
            uint32_t CRYPLPEN : 1;
            uint32_t HASHLPEN : 1;
            uint32_t RNGLPEN : 1;
            uint32_t OTGFSLPEN : 1;
            uint32_t : 24;
        } b;
    } AHB2LPENR;
    union AHB3LPENR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t FMCLPEN : 1;
            uint32_t : 31;
        } b;
    } AHB3LPENR;
    uint32_t RESERVED4;
    union APB1LPENR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t TIM2LPEN : 1;
            uint32_t TIM3LPEN : 1;
            uint32_t TIM4LPEN : 1;
            uint32_t TIM5LPEN : 1;
            uint32_t TIM6LPEN : 1;
            uint32_t TIM7LPEN : 1;
            uint32_t TIM12LPEN : 1;
            uint32_t TIM13LPEN : 1;
            uint32_t TIM14LPEN : 1;
            uint32_t : 2;
            uint32_t WWDGLPEN : 1;
            uint32_t : 2;
            uint32_t SPI2LPEN : 1;
            uint32_t SPI3LPEN : 1;
            uint32_t : 1;
            uint32_t USART2LPEN : 1;
            uint32_t USART3LPEN : 1;
            uint32_t UART4LPEN : 1;
            uint32_t UART5LPEN : 1;
            uint32_t I2C1LPEN : 1;
            uint32_t I2C2LPEN : 1;
            uint32_t I2C3LPEN : 1;
            uint32_t : 1;
            uint32_t CAN1LPEN : 1;
            uint32_t CAN2LPEN : 1;
            uint32_t : 1;
            uint32_t PWRLPEN : 1;
            uint32_t DACLPEN : 1;
            uint32_t UART7LPEN : 1;
            uint32_t UART8LPEN : 1;
        } b;
    } APB1LPENR;
    union APB2LPENR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t TIM1LPEN : 1;
            uint32_t TIM8LPEN : 1;
            uint32_t : 2;
            uint32_t USART1LPEN : 1;
            uint32_t USART6LPEN : 1;
            uint32_t : 2;
            uint32_t ADC1LPEN : 1;
            uint32_t ADC2LPEN : 1;
            uint32_t ADC3LPEN : 1;
            uint32_t SDIOLPEN : 1;
            uint32_t SPI1LPEN : 1;
            uint32_t SPI4LPEN : 1;
            uint32_t SYSCFGLPEN : 1;
            uint32_t : 1;
            uint32_t TIM9LPEN : 1;
            uint32_t TIM10LPEN : 1;
            uint32_t TIM11LPEN : 1;
            uint32_t : 1;
            uint32_t SPI5LPEN : 1;
            uint32_t SPI6LPEN : 1;
            uint32_t SAI1LPEN : 1;
            uint32_t : 3;
            uint32_t LTDCLPEN : 1;
            uint32_t : 5;
        } b;
    } APB2LPENR;
    uint32_t RESERVED5[2];
    union BDCR
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
    union CSR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t LSION : 1;
            uint32_t LSIRDY : 1;
            uint32_t : 22;
            uint32_t RMVF : 1;
            uint32_t BORRSTF : 1;
            uint32_t PADRSTF : 1;
            uint32_t PORRSTF : 1;
            uint32_t SFTRSTF : 1;
            uint32_t WDGRSTF : 1;
            uint32_t WWDGRSTF : 1;
            uint32_t LPWRRSTF : 1;
        } b;
    } CSR;
    uint32_t RESERVED6[2];
    union SSCGR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t MODPER : 13;
            uint32_t INCSTEP : 15;
            uint32_t : 2;
            uint32_t SPREADSEL : 1;
            uint32_t SSCGEN : 1;
        } b;
    } SSCGR;
    union PLLCFGR PLLI2SCFGR;
    union PLLCFGR PLLSAICFGR;
    union DCKCFGR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t PLLI2SDIVQ : 5;
            uint32_t : 3;
            uint32_t PLLSAIDIVQ : 5;
            uint32_t : 3;
            uint32_t PLLSAIDIVR : 2;
            uint32_t : 2;
            uint32_t SAI1ASRC : 2;
            uint32_t SAI1BSRC : 2;
            uint32_t TIMPRE : 1;
            uint32_t : 7;
        } b;
    } DCKCFGR;
};

#define STM32_RCC_REGS (*((volatile STM32_RCC_REG_TYPE*)0x40023800))

