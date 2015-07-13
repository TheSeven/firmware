#pragma once

struct __attribute__((packed)) STM32_USB_REG_TYPE
{
    union EPR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t EA : 4;
            enum STAT
            {
                STAT_DISABLED = 0,
                STAT_STALL = 1,
                STAT_NAK = 2,
                STAT_VALID = 3,
            } STAT_TX : 2;
            uint32_t DTOG_TX : 1;
            uint32_t CTR_TX : 1;
            uint32_t EP_KIND : 1;
            enum EP_TYPE
            {
                EP_TYPE_BULK = 0,
                EP_TYPE_CONTROL = 1,
                EP_TYPE_ISO = 2,
                EP_TYPE_INTERRUPT = 3,
            } EP_TYPE : 2;
            uint32_t SETUP : 1;
            enum STAT STAT_RX : 2;
            uint32_t DTOG_RX : 1;
            uint32_t CTR_RX : 1;
            uint32_t : 16;
        } b;
    } EPR[8];
    uint32_t reserved[8];
    union CNTR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t FRES : 1;
            uint32_t PWDN : 1;
            uint32_t LP_MODE : 1;
            uint32_t FSUSP : 1;
            uint32_t RESUME : 1;
            uint32_t L1RESUME : 1;
            uint32_t : 1;
            uint32_t L1REQM : 1;
            uint32_t ESOFM : 1;
            uint32_t SOFM : 1;
            uint32_t RESETM : 1;
            uint32_t SUSPM : 1;
            uint32_t WKUPM : 1;
            uint32_t ERRM : 1;
            uint32_t PMAOVRM : 1;
            uint32_t CTRM : 1;
            uint32_t : 16;
        } b;
    } CNTR;
    union ISTR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t EP_ID : 4;
            uint32_t DIR : 1;
            uint32_t : 2;
            uint32_t L1REQ : 1;
            uint32_t ESOF : 1;
            uint32_t SOF : 1;
            uint32_t RESET : 1;
            uint32_t SUSP : 1;
            uint32_t WKUP : 1;
            uint32_t ERR : 1;
            uint32_t PMAOVR : 1;
            uint32_t CTR : 1;
            uint32_t : 16;
        } b;
    } ISTR;
    union FNR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t RN : 11;
            uint32_t LSOF : 2;
            uint32_t LCK : 1;
            uint32_t RXDM : 1;
            uint32_t RXDP : 1;
            uint32_t : 16;
        } b;
    } FNR;
    union DADDR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t ADD : 7;
            uint32_t EF : 1;
            uint32_t : 24;
        } b;
    } DADDR;
    uint32_t BTABLE;
    union LPMCSR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t LPMEN : 1;
            uint32_t LPMACK : 1;
            uint32_t : 1;
            uint32_t REMWAKE : 1;
            uint32_t BESL : 4;
            uint32_t : 24;
        } b;
    } LPMCSR;
    union BCDR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t BCDEN : 1;
            uint32_t DCDEN : 1;
            uint32_t PDEN : 1;
            uint32_t SDEN : 1;
            uint32_t DCDET : 1;
            uint32_t PDET : 1;
            uint32_t SDET : 1;
            uint32_t PS2DET : 1;
            uint32_t : 7;
            uint32_t DPPU : 1;
            uint32_t : 16;
        } b;
    } BCDR;
};

struct __attribute__((packed)) STM32_USB_BD_TYPE
{
    uint16_t ADDR_TX_DT0;
    union __attribute__((packed)) COUNT
    {
        uint16_t d16;
        struct
        {
            uint16_t COUNT : 10;
            uint16_t NUM_BLOCK : 5;
            uint16_t BLSIZE : 1;
        } b;
    } COUNT_TX_DT0;
    uint16_t ADDR_RX_DT1;
    union COUNT COUNT_RX_DT1;
};

#define STM32_USB_RAM_BASE 0x40006000
#define STM32_USB_REGS (*((volatile STM32_USB_REG_TYPE*)0x40005c00))
#define STM32_USB_RAM ((volatile uint32_t*)STM32_USB_RAM_BASE)
#define STM32_USB_BDT ((volatile STM32_USB_BD_TYPE*)(STM32_USB_RAM_BASE))
