#pragma once

struct __attribute__((packed)) STM32_AFIO_REG_TYPE
{
    union EVCR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t PIN : 4;
            uint32_t PORT : 3;
            uint32_t EVOE : 1;
            uint32_t : 24;
        } b;
    } EVCR;
    union MAPR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            enum SPI1_REMAP
            {
                SPI1_PA4_7 = 0x0,
                SPI1_PA15_PB3_5 = 0x1,
            } SPI1_REMAP : 1;
            enum I2C1_REMAP
            {
                I2C1_PB6_7 = 0x0,
                I2C1_PB8_9 = 0x1,
            } I2C1_REMAP : 1;
            enum USART1_REMAP
            {
                USART1_PA9_10 = 0x0,
                USART1_PB6_7 = 0x1,
            } USART1_REMAP : 1;
            enum USART2_REMAP
            {
                USART2_PA0_4 = 0x0,
                USART2_PD3_7 = 0x1,
            } USART2_REMAP : 1;
            enum USART3_REMAP
            {
                USART3_PB10_14 = 0x0,
                USART3_PC10_12_PB13_14 = 0x1,
                USART3_PD8_12 = 0x3,
            } USART3_REMAP : 2;
            enum TIM1_REMAP
            {
                TIM1_PA8_12_PB12_15 = 0x0,
                TIM1_PA6_12_PB0_1 = 0x1,
                TIM1_PE7_15 = 0x3,
            } TIM1_REMAP : 2;
            enum TIM2_REMAP
            {
                TIM2_PA0_3 = 0x0,
                TIM2_PA2_3_PA15_PB3 = 0x1,
                TIM2_PA0_1_PB10_11 = 0x2,
                TIM2_PA15_PB3_PB10_11 = 0x3,
            } TIM2_REMAP : 2;
            enum TIM3_REMAP
            {
                TIM3_PA6_7_PB0_1 = 0x0,
                TIM3_PB0_1_PB4_5 = 0x2,
                TIM3_PC6_9 = 0x3,
            } TIM3_REMAP : 2;
            enum TIM4_REMAP
            {
                TIM4_PB6_9 = 0x0,
                TIM4_PD12_15 = 0x1,
            } TIM4_REMAP : 1;
            enum CAN1_REMAP
            {
                CAN1_PA11_12 = 0x0,
                CAN1_PB8_9 = 0x2,
                CAN1_PD0_1 = 0x3,
            } CAN1_REMAP : 2;
            enum PD01_REMAP
            {
                PD01_NORMAL = 0x0,
                PD01_OSCINOUT = 0x1,
            } PD01_REMAP : 1;
            enum TIM5CH4_IREMAP
            {
                TIM5CH4_PA3 = 0x0,
                TIM5CH4_LSI = 0x1,
            } TIM5CH4_IREMAP : 1;
            enum ADC1_ETRGINJ_REMAP
            {
                ADC1_ETRGINJ_EXTI11 = 0x0,
                ADC1_ETRGINJ_TIM8_TRGO = 0x1,
            } ADC1_ETRGINJ_REMAP : 1;
            enum ADC1_ETRGREG_REMAP
            {
                ADC1_ETRGREG_EXTI11 = 0x0,
                ADC1_ETRGREG_TIM8_CH4 = 0x1,
            } ADC1_ETRGREG_REMAP : 1;
            enum ADC2_ETRGINJ_REMAP
            {
                ADC2_ETRGINJ_EXTI11 = 0x0,
                ADC2_ETRGINJ_TIM8_TRGO = 0x1,
            } ADC2_ETRGINJ_REMAP : 1;
            enum ADC2_ETRGREG_REMAP
            {
                ADC2_ETRGREG_EXTI15 = 0x0,
                ADC2_ETRGREG_TIM8_CH4 = 0x1,
            } ADC2_ETRGREG_REMAP : 1;
            enum ETH_REMAP
            {
                ETH_PA7_PC4_5_PB0_1 = 0x0,
                ETH_PD8_12 = 0x1,
            } ETH_REMAP : 1;
            enum CAN2_REMAP
            {
                CAN2_PB12_13 = 0x0,
                CAN2_PB5_6 = 0x1,
            } CAN2_REMAP : 1;
            enum MII_RMII
            {
                MII_SEL = 0x0,
                RMII_SEL = 0x1,
            } MII_RMII : 1;
            enum
            {
                SWJ_FULL = 0x0,
                SWJ_FULL_NO_NJTRST = 0x1,
                SWJ_SWI_ONLY = 0x2,
                SWJ_DISABLED = 0x4,
            } SWJ_CFG : 3;
            uint32_t : 1;
            enum SPI3_REMAP
            {
                SPI3_PA15_PB3_5 = 0x0,
                SPI3_PA4_PC10_12 = 0x1,
            } SPI3_REMAP : 1;
            enum TIM2ITR1_REMAP
            {
                TIM2ITR1_ETH_PTP = 0x0,
                TIM2ITR1_USB_SOF = 0x1,
            } TIM2ITR1_REMAP : 1;
            enum PTP_PPS_REMAP
            {
                PTP_PPS_OFF = 0x0,
                PTP_PPS_PB5 = 0x1,
            } PTP_PPS_REMAP : 1;
            uint32_t : 1;
        } b;
    } MAPR;
    union EXTICR1
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t EXTI0 : 4;
            uint32_t EXTI1 : 4;
            uint32_t EXTI2 : 4;
            uint32_t EXTI3 : 4;
            uint32_t : 16;
        } b;
    } EXTICR1;
    union EXTICR2
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t EXTI4 : 4;
            uint32_t EXTI5 : 4;
            uint32_t EXTI6 : 4;
            uint32_t EXTI7 : 4;
            uint32_t : 16;
        } b;
    } EXTICR2;
    union EXTICR3
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t EXTI8 : 4;
            uint32_t EXTI9 : 4;
            uint32_t EXTI10 : 4;
            uint32_t EXTI11 : 4;
            uint32_t : 16;
        } b;
    } EXTICR3;
    union EXTICR4
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t EXTI12 : 4;
            uint32_t EXTI13 : 4;
            uint32_t EXTI14 : 4;
            uint32_t EXTI15 : 4;
            uint32_t : 16;
        } b;
    } EXTICR4;
    uint32_t RESERVED;
    union MAPR2
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            enum TIM15_REMAP
            {
                TIM15_0 = 0x0,
                TIM15_1 = 0x1,
            } TIM15_REMAP : 1;
            enum TIM16_REMAP
            {
                TIM16_0 = 0x0,
                TIM16_1 = 0x1,
            } TIM16_REMAP : 1;
            enum TIM17_REMAP
            {
                TIM17_0 = 0x0,
                TIM17_1 = 0x1,
            } TIM17_REMAP : 1;
            enum CEC_REMAP
            {
                CEC_0 = 0x0,
                CEC_1 = 0x1,
            } CEC_REMAP : 1;
            enum TIM1_DMA_REMAP
            {
                TIM1_DMA_0 = 0x0,
                TIM1_DMA_1 = 0x1,
            } TIM1_DMA_REMAP : 1;
            enum TIM9_REMAP
            {
                TIM9_PA2_3 = 0x0,
                TIM9_PE5_6 = 0x1,
            } TIM9_REMAP : 1;
            enum TIM10_REMAP
            {
                TIM10_PB8 = 0x0,
                TIM10_PF6 = 0x1,
            } TIM10_REMAP : 1;
            enum TIM11_REMAP
            {
                TIM11_PB9 = 0x0,
                TIM11_PF7 = 0x1,
            } TIM11_REMAP : 1;
            enum TIM13_REMAP
            {
                TIM13_PA6 = 0x0,
                TIM13_PF8 = 0x1,
            } TIM13_REMAP : 1;
            enum TIM14_REMAP
            {
                TIM14_PA7 = 0x0,
                TIM14_PF9 = 0x1,
            } TIM14_REMAP : 1;
            enum FSMC_NADV
            {
                FSMC_NADV_NORMAL = 0x0,
                FSMC_NADV_DISABLE = 0x1,
            } FSMC_NADV : 1;
            enum TIM67_DAC_DMA_REMAP
            {
                TIM67_DAC_DMA_0 = 0x0,
                TIM67_DAC_DMA_1 = 0x1,
            } TIM67_DAC_DMA_REMAP : 1;
            enum TIM12_REMAP
            {
                TIM12_0 = 0x0,
                TIM12_1 = 0x1,
            } TIM12_REMAP : 1;
            enum MISC_REMAP
            {
                MISC_0 = 0x0,
                MISC_1 = 0x1,
            } MISC_REMAP : 1;
            uint32_t : 18;
        } b;
    } MAPR2;
};

#define STM32_AFIO_REGS (*((volatile STM32_AFIO_REG_TYPE*)0x40010000))

