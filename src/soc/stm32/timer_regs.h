#pragma once

struct __attribute__((packed)) STM32_TIM_REG_TYPE
{
    union CR1
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t CEN : 1;
            uint32_t UDIS : 1;
            uint32_t URS : 1;
            uint32_t OPM : 1;
            uint32_t DIR : 1;
            uint32_t CMS : 2;
            uint32_t ARPE : 1;
            uint32_t CKD : 2;
            uint32_t : 22;
        } b;
    } CR1;
    union CR2
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t CCPC : 1;
            uint32_t : 1;
            uint32_t CCUS : 1;
            uint32_t CCDS : 1;
            uint32_t MMS : 3;
            uint32_t TI1S : 1;
            uint32_t OIS1 : 1;
            uint32_t OIS1N : 1;
            uint32_t OIS2 : 1;
            uint32_t OIS2N : 1;
            uint32_t OIS3 : 1;
            uint32_t OIS3N : 1;
            uint32_t OIS4 : 1;
            uint32_t : 17;
        } b;
    } CR2;
    union SMCR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t SMS : 3;
            uint32_t : 1;
            uint32_t TS : 3;
            uint32_t MSM : 1;
            uint32_t ETF : 4;
            uint32_t ETPS : 2;
            uint32_t ECE : 1;
            uint32_t ETP : 1;
            uint32_t : 16;
        } b;
    } SMCR;
    union DIER
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t UIE : 1;
            uint32_t CC1IE : 1;
            uint32_t CC2IE : 1;
            uint32_t CC3IE : 1;
            uint32_t CC4IE : 1;
            uint32_t COMIE : 1;
            uint32_t TIE : 1;
            uint32_t BIE : 1;
            uint32_t UDE : 1;
            uint32_t CC1DE : 1;
            uint32_t CC2DE : 1;
            uint32_t CC3DE : 1;
            uint32_t CC4DE : 1;
            uint32_t COMDE : 1;
            uint32_t TDE : 1;
            uint32_t : 17;
        } b;
    } DIER;
    union SR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t UIF : 1;
            uint32_t CC1IF : 1;
            uint32_t CC2IF : 1;
            uint32_t CC3IF : 1;
            uint32_t CC4IF : 1;
            uint32_t COMIF : 1;
            uint32_t TIF : 1;
            uint32_t BIF : 1;
            uint32_t : 1;
            uint32_t CC1OF : 1;
            uint32_t CC2OF : 1;
            uint32_t CC3OF : 1;
            uint32_t CC4OF : 1;
            uint32_t : 19;
        } b;
    } SR;
    union EGR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t UG : 1;
            uint32_t CC1G : 1;
            uint32_t CC2G : 1;
            uint32_t CC3G : 1;
            uint32_t CC4G : 1;
            uint32_t COMG : 1;
            uint32_t TG : 1;
            uint32_t BG : 1;
            uint32_t : 24;
        } b;
    } EGR;
    union CCMR1
    {
        uint32_t d32;
        struct __attribute__((packed)) oc
        {
            uint32_t CC1S : 2;
            uint32_t OC1FE : 1;
            uint32_t OC1PE : 1;
            uint32_t OC1M : 3;
            uint32_t OC1CE : 1;
            uint32_t CC2S : 2;
            uint32_t OC2FE : 1;
            uint32_t OC2PE : 1;
            uint32_t OC2M : 3;
            uint32_t OC2CE : 1;
            uint32_t : 16;
        } oc;
        struct __attribute__((packed)) ic
        {
            uint32_t CC1S : 2;
            uint32_t IC1PSC : 2;
            uint32_t IC1F : 4;
            uint32_t CC2S : 2;
            uint32_t IC2PSC : 2;
            uint32_t IC2F : 4;
            uint32_t : 16;
        } ic;
    } CCMR1;
    union CCMR2
    {
        uint32_t d32;
        struct __attribute__((packed)) oc
        {
            uint32_t CC3S : 2;
            uint32_t OC3FE : 1;
            uint32_t OC3PE : 1;
            uint32_t OC3M : 3;
            uint32_t OC3CE : 1;
            uint32_t CC4S : 2;
            uint32_t OC4FE : 1;
            uint32_t OC4PE : 1;
            uint32_t OC4M : 3;
            uint32_t OC4CE : 1;
            uint32_t : 16;
        } oc;
        struct __attribute__((packed)) ic
        {
            uint32_t CC3S : 2;
            uint32_t IC3PSC : 2;
            uint32_t IC3F : 4;
            uint32_t CC4S : 2;
            uint32_t IC4PSC : 2;
            uint32_t IC4F : 4;
            uint32_t : 16;
        } ic;
    } CCMR2;
    union CCER
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t CC1E : 1;
            uint32_t CC1P : 1;
            uint32_t CC1NE : 1;
            uint32_t CC1NP : 1;
            uint32_t CC2E : 1;
            uint32_t CC2P : 1;
            uint32_t CC2NE : 1;
            uint32_t CC2NP : 1;
            uint32_t CC3E : 1;
            uint32_t CC3P : 1;
            uint32_t CC3NE : 1;
            uint32_t CC3NP : 1;
            uint32_t CC4E : 1;
            uint32_t CC4P : 1;
            uint32_t CC4NE : 1;
            uint32_t CC4NP : 1;
            uint32_t : 16;
        } b;
    } CCER;
    uint32_t CNT;
    uint32_t PSC;
    uint32_t ARR;
    uint32_t RCR;
    uint32_t CCR1;
    uint32_t CCR2;
    uint32_t CCR3;
    uint32_t CCR4;
    union BDTR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t DTG : 8;
            uint32_t LOCK : 2;
            uint32_t OSSI : 1;
            uint32_t OSSR : 1;
            uint32_t BKE : 1;
            uint32_t BKP : 1;
            uint32_t AOE : 1;
            uint32_t MOE : 1;
            uint32_t : 16;
        } b;
    } BDTR;
    union DCR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t DBA : 5;
            uint32_t : 3;
            uint32_t DBL : 5;
            uint32_t : 19;
        } b;
    } DCR;
    uint32_t DMAR;
    union OR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t TI1_RMP : 2;
            uint32_t : 4;
            uint32_t TI4_RMP : 2;
            uint32_t : 2;
            uint32_t ITR1_RMP : 2;
            uint32_t : 20;
        } b;
    } OR;
};

#ifdef SOC_STM32F0
#define STM32_TIM1_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40012c00))
#define STM32_TIM2_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40000000))
#define STM32_TIM3_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40000400))
#define STM32_TIM6_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40001000))
#define STM32_TIM7_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40001400))
#define STM32_TIM14_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40002000))
#define STM32_TIM15_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40014000))
#define STM32_TIM16_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40014400))
#define STM32_TIM17_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40014800))
#else
#define STM32_TIM1_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40010000))
#define STM32_TIM2_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40000000))
#define STM32_TIM3_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40000400))
#define STM32_TIM4_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40000800))
#define STM32_TIM5_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40000c00))
#define STM32_TIM6_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40001000))
#define STM32_TIM7_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40001400))
#define STM32_TIM8_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40010400))
#define STM32_TIM9_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40014000))
#define STM32_TIM10_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40014400))
#define STM32_TIM11_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40014800))
#define STM32_TIM12_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40001800))
#define STM32_TIM13_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40001c00))
#define STM32_TIM14_REGS (*((volatile STM32_TIM_REG_TYPE*)0x40002000))
#endif
