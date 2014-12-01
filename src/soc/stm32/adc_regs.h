#pragma once

struct __attribute__((packed)) STM32_ADC_REG_TYPE
{
    union SR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t AWD : 1;
            uint32_t EOC : 1;
            uint32_t JEOC : 1;
            uint32_t JSTRT : 1;
            uint32_t STRT : 1;
            uint32_t OVR : 1;
            uint32_t : 26;
        } b;
    } SR;
    union CR1
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t AWDCH : 5;
            uint32_t EOCIE : 1;
            uint32_t AWDIE : 1;
            uint32_t JEOCIE : 1;
            uint32_t SCAN : 1;
            uint32_t AWDSGL : 1;
            uint32_t JAUTO : 1;
            uint32_t DISCEN : 1;
            uint32_t JDISCEN : 1;
            uint32_t DISCNUM : 3;
            uint32_t : 6;
            uint32_t JAWDEN : 1;
            uint32_t AWDEN : 1;
            uint32_t RES : 2;
            uint32_t OVRIE : 1;
            uint32_t : 5;
        } b;
    } CR1;
    union CR2
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t ADON : 1;
            uint32_t CONT : 1;
            uint32_t : 6;
            uint32_t DMA : 1;
            uint32_t DDS : 1;
            uint32_t EOCS : 1;
            uint32_t ALIGN : 1;
            uint32_t : 4;
            uint32_t JEXTSEL : 4;
            uint32_t JEXTEN : 2;
            uint32_t JSWSTART : 1;
            uint32_t : 1;
            uint32_t EXTSEL : 4;
            uint32_t EXTEN : 2;
            uint32_t SWSTART : 1;
            uint32_t : 1;
        } b;
    } CR2;
    union SMPR1
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t SMP10 : 3;
            uint32_t SMP11 : 3;
            uint32_t SMP12 : 3;
            uint32_t SMP13 : 3;
            uint32_t SMP14 : 3;
            uint32_t SMP15 : 3;
            uint32_t SMP16 : 3;
            uint32_t SMP17 : 3;
            uint32_t SMP18 : 3;
            uint32_t : 5;
        } b;
    } SMPR1;
    union SMPR2
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t SMP0 : 3;
            uint32_t SMP1 : 3;
            uint32_t SMP2 : 3;
            uint32_t SMP3 : 3;
            uint32_t SMP4 : 3;
            uint32_t SMP5 : 3;
            uint32_t SMP6 : 3;
            uint32_t SMP7 : 3;
            uint32_t SMP8 : 3;
            uint32_t SMP9 : 3;
            uint32_t : 2;
        } b;
    } SMPR2;
    uint32_t JOFR1;
    uint32_t JOFR2;
    uint32_t JOFR3;
    uint32_t JOFR4;
    uint32_t HTR;
    uint32_t LTR;
    union SQR1
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t SQ13 : 5;
            uint32_t SQ14 : 5;
            uint32_t SQ15 : 5;
            uint32_t SQ16 : 5;
            uint32_t L : 4;
            uint32_t : 8;
        } b;
    } SQR1;
    union SQR2
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t SQ7 : 5;
            uint32_t SQ8 : 5;
            uint32_t SQ9 : 5;
            uint32_t SQ10 : 5;
            uint32_t SQ11 : 5;
            uint32_t SQ12 : 5;
            uint32_t : 2;
        } b;
    } SQR2;
    union SQR3
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t SQ1 : 5;
            uint32_t SQ2 : 5;
            uint32_t SQ3 : 5;
            uint32_t SQ4 : 5;
            uint32_t SQ5 : 5;
            uint32_t SQ6 : 5;
            uint32_t : 2;
        } b;
    } SQR3;
    union JSQR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t JSQ1 : 5;
            uint32_t JSQ2 : 5;
            uint32_t JSQ3 : 5;
            uint32_t JSQ4 : 5;
            uint32_t JL : 2;
            uint32_t : 10;
        } b;
    } JSQR;
    uint32_t JDR1;
    uint32_t JDR2;
    uint32_t JDR3;
    uint32_t JDR4;
    union DR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t DATA : 16;
            uint32_t ADC2DATA : 16;
        } b;
    } DR;
};

#define STM32_ADC2_REGS (*((volatile STM32_ADC_REG_TYPE*)0x40012100))
#define STM32_ADC3_REGS (*((volatile STM32_ADC_REG_TYPE*)0x40012200))
#define STM32_ADC1_REGS (*((volatile STM32_ADC_REG_TYPE*)0x40012000))
