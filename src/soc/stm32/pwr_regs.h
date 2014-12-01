#pragma once

struct __attribute__((packed)) STM32_PWR_REG_TYPE
{
    union CR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t LPDS : 1;
            uint32_t PDDS : 1;
            uint32_t CWUF : 1;
            uint32_t CSBF : 1;
            uint32_t PVDE : 1;
            uint32_t PLS : 3;
            uint32_t DBP : 1;
            uint32_t FPDS : 1;
            uint32_t LPLVDS : 1;
            uint32_t MRUDS : 1;
            uint32_t : 1;
            uint32_t ADCDC1 : 1;
            uint32_t PMODE : 2;
            uint32_t ODEN : 1;
            uint32_t ODSWEN : 1;
            uint32_t UDEN : 2;
            uint32_t : 12;
        } b;
    } CR;
    union CSR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t WUF : 1;
            uint32_t SBF : 1;
            uint32_t PVDO : 1;
            uint32_t BRR : 1;
            uint32_t : 4;
            uint32_t EWUP : 1;
            uint32_t BRE : 1;
            uint32_t : 4;
            uint32_t REGRDY : 1;
            uint32_t : 1;
            uint32_t ODRDY : 1;
            uint32_t ODSWRDY : 1;
            uint32_t UDSWRDY : 2;
            uint32_t : 12;
        } b;
    } CSR;
};

#define STM32_PWR_REGS (*((volatile STM32_PWR_REG_TYPE*)0x40007000))

