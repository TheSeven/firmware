#pragma once

struct __attribute__((packed)) STM32_SYSCFG_REG_TYPE
{
    union CFGR1
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t MEM_MODE : 3;
            uint32_t : 1;
            uint32_t PA11_PA12_RMP : 1;
            uint32_t : 3;
#ifdef SOC_STM32F4
            uint32_t UFB_MODE : 1;
#else
            uint32_t ADC_DMA_RMP : 1;
#endif
            uint32_t USART1_TX_DMA_RMP : 1;
            uint32_t USART1_RX_DMA_RMP : 1;
            uint32_t TIM16_DMA_RMP : 1;
            uint32_t TIM17_DMA_RMP : 1;
            uint32_t : 3;
            uint32_t I2C_PB6_FMP : 1;
            uint32_t I2C_PB7_FMP : 1;
            uint32_t I2C_PB8_FMP : 1;
            uint32_t I2C_PB9_FMP : 1;
            uint32_t I2C1_FMP : 1;
            uint32_t : 1;
            uint32_t I2C1_PA9_FMP : 1;
            uint32_t I2C1_PA10_FMP : 1;
            uint32_t : 2;
            uint32_t USART3_DMA_RMP : 1;
            uint32_t : 5;
        } b;
    } CFGR1;
    union PMC
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t : 16;
            uint32_t ADCxDC2 : 3;
            uint32_t : 4;
            enum
            {
                MII_SEL = 0x0,
                RMII_SEL = 0x1,
            } MII_RMII : 1;
            uint32_t : 8;
        } b;
    } PMC;
    uint32_t EXTICR[4];
    uint32_t : 32;
    union CFGR2
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t LOCKUP_LOCK : 1;
            uint32_t SRAM_PARITY_LOCK : 1;
            uint32_t : 6;
            uint32_t SRAM_PEF : 1;
            uint32_t : 23;
        } b;
    } CFGR2;
    union CMPCR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t CMP_PD : 1;
            uint32_t : 7;
            uint32_t READY : 1;
            uint32_t : 23;
        } b;
    } CMPCR;
};

#ifdef SOC_STM32F0
#define STM32_SYSCFG_REGS (*((volatile STM32_SYSCFG_REG_TYPE*)0x40010000))
#else
#define STM32_SYSCFG_REGS (*((volatile STM32_SYSCFG_REG_TYPE*)0x40013800))
#endif
