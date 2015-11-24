#pragma once

#include "global.h"

struct __attribute__((packed)) STM32_FLASH_REG_TYPE
{
    union ACR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t LATENCY : 4;
            uint32_t PRFTBE : 1;
            uint32_t PRFTBS : 1;
            uint32_t : 2;
            uint32_t PRFTEN : 1;
            uint32_t ICEN : 1;
            uint32_t DCEN : 1;
            uint32_t ICRST : 1;
            uint32_t DCRST : 1;
            uint32_t : 19;
        } b;
    } ACR;
    uint32_t KEYR;
    uint32_t OPTKEYR;
#if defined(SOC_STM32F0) || defined(SOC_STM32F1)
    union SR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t BSY : 1;
            uint32_t : 1;
            uint32_t PGERR : 1;
            uint32_t : 1;
            uint32_t WRPRTERR : 1;
            uint32_t EOP : 1;
            uint32_t : 26;
        } b;
    } SR;
    union CR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t PG : 1;
            uint32_t PER : 1;
            uint32_t MER : 1;
            uint32_t : 1;
            uint32_t OPTPG : 1;
            uint32_t OPTER : 1;
            uint32_t STRT : 1;
            uint32_t LOCK : 1;
            uint32_t : 1;
            uint32_t OPTWRE : 1;
            uint32_t ERRIE : 1;
            uint32_t : 1;
            uint32_t EOPIE : 1;
            uint32_t OBL_LAUNCH : 1;
            uint32_t : 18;
        } b;
    } CR;
    uint32_t AR;
    union OBR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t OPTERR : 1;
            uint32_t RDPRT : 2;
            uint32_t : 5;
            uint32_t WDG_SW : 1;
            uint32_t nRST_STOP : 1;
            uint32_t nRST_STDBY : 1;
            uint32_t BOOT0 : 1;
            uint32_t nBOOT1 : 1;
            uint32_t VDDA_MONITOR : 1;
            uint32_t RAM_PARITY_CHECK : 1;
            uint32_t BOOT_SEL : 1;
            uint32_t DATA0 : 8;
            uint32_t DATA1 : 8;
        } b;
    } OBR;
    uint32_t WRPR;
#else
    union SR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t EOP : 1;
            uint32_t SOP : 1;
            uint32_t : 2;
            uint32_t WRPERR : 1;
            uint32_t PGAERR : 1;
            uint32_t PGPERR : 1;
            uint32_t PGSERR : 1;
            uint32_t : 8;
            uint32_t BSY : 1;
            uint32_t : 15;
        } b;
    } SR;
    union CR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t PG : 1;
            uint32_t SER : 1;
            uint32_t MER : 1;
            uint32_t SNB : 5;
            uint32_t PSIZE : 2;
            uint32_t : 5;
            uint32_t MER2 : 1;
            uint32_t STRT : 1;
            uint32_t : 7;
            uint32_t EOPIE : 1;
            uint32_t : 6;
            uint32_t LOCK : 1;
        } b;
    } CR;
    union OPTCR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t OPTLOCK : 1;
            uint32_t OPTSTRT : 1;
            uint32_t BOR_LEV : 2;
            uint32_t BFB2 : 1;
            uint32_t WDG_SW : 1;
            uint32_t nRST_STOP : 1;
            uint32_t nRST_STDBY : 1;
            uint32_t RDP : 8;
            uint32_t nWRP : 12;
            uint32_t : 2;
            uint32_t DB1M : 1;
            uint32_t SPRMOD : 1;
        } b;
    } OPTCR;
    union OPTCR1
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t : 16;
            uint32_t nWRP : 12;
            uint32_t : 4;
        } b;
    } OPTCR1;
#endif
};

#if defined(SOC_STM32F0) || defined(SOC_STM32F1)
#define STM32_FLASH_REGS (*((volatile STM32_FLASH_REG_TYPE*)0x40022000))
#else
#define STM32_FLASH_REGS (*((volatile STM32_FLASH_REG_TYPE*)0x40023c00))
#endif
