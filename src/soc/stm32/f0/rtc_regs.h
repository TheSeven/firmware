#pragma once

struct __attribute__((packed)) STM32_RTC_REG_TYPE
{
    union TR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t SU : 4;
            uint32_t ST : 3;
            uint32_t : 1;
            uint32_t MNU : 4;
            uint32_t MNT : 3;
            uint32_t : 1;
            uint32_t HU : 4;
            uint32_t HT : 2;
            uint32_t PM : 1;
            uint32_t : 9;
        } b;
    } TR;
    union DR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t DU : 4;
            uint32_t DT : 2;
            uint32_t : 2;
            uint32_t MU : 4;
            uint32_t MT : 1;
            uint32_t WDU : 3;
            uint32_t YU : 4;
            uint32_t YT : 4;
            uint32_t : 8;
        } b;
    } DR;
    union CR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t WUCKSEL : 3;
            uint32_t TSEDGE : 1;
            uint32_t REFCKON : 1;
            uint32_t BYPSHAD : 1;
            uint32_t FMT : 1;
            uint32_t : 1;
            uint32_t ALRAE : 1;
            uint32_t : 1;
            uint32_t WUTE : 1;
            uint32_t TSE : 1;
            uint32_t ALRAIE : 1;
            uint32_t : 1;
            uint32_t WUTIE : 1;
            uint32_t TSIE : 1;
            uint32_t ADD1H : 1;
            uint32_t SUB1H : 1;
            uint32_t BCK : 1;
            uint32_t COSEL : 1;
            uint32_t POL : 1;
            uint32_t OSEL : 2;
            uint32_t COE : 1;
            uint32_t : 8;
        } b;
    } CR;
    union ISR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t ALRAWF : 1;
            uint32_t : 2;
            uint32_t SHPF : 1;
            uint32_t INITS : 1;
            uint32_t RSF : 1;
            uint32_t INITF : 1;
            uint32_t INIT : 1;
            uint32_t ALRAF : 1;
            uint32_t : 1;
            uint32_t WUTF : 1;
            uint32_t TSF : 1;
            uint32_t TSOVF : 1;
            uint32_t TAMP1F : 1;
            uint32_t TAMP2F : 1;
            uint32_t : 1;
            uint32_t RECALPF : 1;
            uint32_t : 15;
        } b;
    } ISR;
    union PRER
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t PREDIV_S : 15;
            uint32_t : 1;
            uint32_t PREDIV_A : 7;
            uint32_t : 9;
        } b;
    } PRER;
    uint32_t WUTR;
    uint32_t RESERVED2;
    union ALRMAR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t SU : 4;
            uint32_t ST : 3;
            uint32_t MSK1 : 1;
            uint32_t MNU : 4;
            uint32_t MNT : 3;
            uint32_t MSK2 : 1;
            uint32_t HU : 4;
            uint32_t HT : 2;
            uint32_t PM : 1;
            uint32_t MSK3 : 1;
            uint32_t DU : 4;
            uint32_t DT : 2;
            uint32_t WDSEL : 1;
            uint32_t MSK4 : 1;
        } b;
    } ALRMAR;
    uint32_t RESERVED3;
    uint32_t WPR;
    uint32_t SSR;
    union SHIFTR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t SUBFS : 15;
            uint32_t : 16;
            uint32_t ADD1S : 1;
        } b;
    } SHIFTR;
    union TSTR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t SU : 4;
            uint32_t ST : 3;
            uint32_t : 1;
            uint32_t MNU : 4;
            uint32_t MNT : 3;
            uint32_t : 1;
            uint32_t HU : 4;
            uint32_t HT : 2;
            uint32_t PM : 1;
            uint32_t : 9;
        } b;
    } TSTR;
    union TSDR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t DU : 4;
            uint32_t DT : 2;
            uint32_t : 2;
            uint32_t MU : 4;
            uint32_t MT : 1;
            uint32_t WDU : 3;
            uint32_t : 16;
        } b;
    } TSDR;
    uint32_t TSSSR;
    union CALR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t CALM : 9;
            uint32_t : 4;
            uint32_t CALW16 : 1;
            uint32_t CALW8 : 1;
            uint32_t CALP : 1;
            uint32_t : 16;
        } b;
    } CALR;
    union TAFCR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t TAMP1E : 1;
            uint32_t TAMP1TRG : 1;
            uint32_t TAMPIE : 1;
            uint32_t TAMP2E : 1;
            uint32_t TAMP2TRG : 1;
            uint32_t : 2;
            uint32_t TAMPTS : 1;
            uint32_t TAMPFREQ : 3;
            uint32_t TAMPFLT : 2;
            uint32_t TAMPPRCH : 2;
            uint32_t TAMPPUDIS : 1;
            uint32_t : 2;
            uint32_t ALARMOUTTYPE : 1;
            uint32_t : 13;
        } b;
    } TAFCR;
    union ALRMASSR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t SS : 15;
            uint32_t : 9;
            uint32_t MASKSS : 4;
            uint32_t : 4;
        } b;
    } ALRMASSR;
};

#define STM32_RTC_REGS (*((volatile STM32_RTC_REG_TYPE*)0x40002800))
