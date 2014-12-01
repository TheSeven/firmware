#pragma once

struct __attribute__((packed)) STM32_ADC_COMMON_REG_TYPE
{
    union CCR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t : 22;
            uint32_t VREFEN : 1;
            uint32_t TSEN : 1;
            uint32_t VBATEN : 1;
            uint32_t : 7;
        } b;
    } CCR;
};

#define STM32_ADC_REGS (*((volatile STM32_ADC_COMMON_REG_TYPE*)0x40012708))

struct __attribute__((packed)) STM32_ADC_REG_TYPE
{
    union ISR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t ADRDY : 1;
            uint32_t EOSMP : 1;
            uint32_t EOC : 1;
            uint32_t EOSEQ : 1;
            uint32_t OVR : 1;
            uint32_t : 2;
            uint32_t AWD : 1;
            uint32_t : 24;
        } b;
    } ISR;
    union IER
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t ADRDYIE : 1;
            uint32_t EOSMPIE : 1;
            uint32_t EOCIE : 1;
            uint32_t EOSIE : 1;
            uint32_t OVRIE : 1;
            uint32_t : 2;
            uint32_t AWDIE : 1;
            uint32_t : 24;
        } b;
    } IER;
    union CR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t ADEN : 1;
            uint32_t ADDIS : 1;
            uint32_t ADSTART : 1;
            uint32_t : 1;
            uint32_t ADSTP : 1;
            uint32_t : 26;
            uint32_t ADCAL : 1;
        } b;
    } CR;
    union CFGR1
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t DMAEN : 1;
            uint32_t DMACFG : 1;
            uint32_t SCANDIR : 1;
            uint32_t RES : 2;
            uint32_t ALIGN : 1;
            uint32_t EXTSEL : 3;
            uint32_t : 1;
            uint32_t EXTEN : 2;
            uint32_t OVRMOD : 1;
            uint32_t CONT : 1;
            uint32_t AUTDLY : 1;
            uint32_t AUTOFF : 1;
            uint32_t DISCEN : 1;
            uint32_t : 5;
            uint32_t AWDSGL : 1;
            uint32_t AWDEN : 1;
            uint32_t : 2;
            uint32_t AWDCH : 5;
            uint32_t : 1;
        } b;
    } CFGR1;
    union CFGR2
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t : 30;
            uint32_t JITOFFDIV2 : 1;
            uint32_t JITOFFDIV4 : 1;
        } b;
    } CFGR2;
    uint32_t SMPR;
    uint32_t RESERVED1;
    uint32_t RESERVED2;
    union TR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t LT : 12;
            uint32_t : 4;
            uint32_t HT : 12;
            uint32_t : 4;
        } b;
    } TR;
    uint32_t RESERVED3;
    union CHSELR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t CHSEL0 : 1;
            uint32_t CHSEL1 : 1;
            uint32_t CHSEL2 : 1;
            uint32_t CHSEL3 : 1;
            uint32_t CHSEL4 : 1;
            uint32_t CHSEL5 : 1;
            uint32_t CHSEL6 : 1;
            uint32_t CHSEL7 : 1;
            uint32_t CHSEL8 : 1;
            uint32_t CHSEL9 : 1;
            uint32_t CHSEL10 : 1;
            uint32_t CHSEL11 : 1;
            uint32_t CHSEL12 : 1;
            uint32_t CHSEL13 : 1;
            uint32_t CHSEL14 : 1;
            uint32_t CHSEL15 : 1;
            uint32_t CHSEL16 : 1;
            uint32_t CHSEL17 : 1;
            uint32_t CHSEL18 : 1;
            uint32_t : 13;
        } b;
    } CHSELR;
    uint32_t RESERVED4[5];
    uint32_t DR;
};

#define STM32_ADC1_REGS (*((volatile STM32_ADC_REG_TYPE*)0x40012400))
