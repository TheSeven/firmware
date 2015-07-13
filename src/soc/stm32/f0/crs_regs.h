#pragma once

struct __attribute__((packed)) STM32_CRS_REG_TYPE
{
    union CR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t SYNCOKIE : 1;
            uint32_t SYNCWARNIE : 1;
            uint32_t ERRIE : 1;
            uint32_t ESYNCIE : 1;
            uint32_t : 1;
            uint32_t CEN : 1;
            uint32_t AUTOTRIMEN : 1;
            uint32_t SWSYNC : 1;
            uint32_t TRIM : 6;
            uint32_t : 18;
        } b;
    } CR;
    union CFGR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t RELOAD : 16;
            uint32_t FELIM : 8;
            uint32_t SYNCDIV : 3;
            uint32_t : 1;
            enum SYNCSRC
            {
                SYNCSRC_GPIO = 0,
                SYNCSRC_LSE = 1,
                SYNCSRC_USB = 2,
            } SYNCSRC : 2;
            uint32_t : 1;
            uint32_t SYNCPOL : 1;
        } b;
    } CFGR;
    union ISR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t SYNCOKF : 1;
            uint32_t SYNCWARNF : 1;
            uint32_t ERRF : 1;
            uint32_t ESYNCF : 1;
            uint32_t : 4;
            uint32_t SYNCERR : 1;
            uint32_t SYNCMISS : 1;
            uint32_t TRIMOVF : 1;
            uint32_t : 4;
            uint32_t FEDIR : 1;
            uint32_t FECAP : 16;
        } b;
    } ISR;
    union ICR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t SYNCOKC : 1;
            uint32_t SYNCWARNC : 1;
            uint32_t ERRC : 1;
            uint32_t ESYNCC : 1;
            uint32_t : 28;
        } b;
    } ICR;
};

#define STM32_CRS_REGS (*((volatile STM32_CRS_REG_TYPE*)0x40006c00))
