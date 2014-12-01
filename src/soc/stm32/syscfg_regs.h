#pragma once

struct __attribute__((packed)) STM32_SYSCFG_REG_TYPE
{
    union MEMRMP
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t MEM_MODE : 3;
            uint32_t : 5;
            uint32_t UFB_MODE : 1;
            uint32_t : 23;
        } b;
    } MEMRMP;
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
    uint32_t RESERVED[2];
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

#define STM32_SYSCFG_REGS (*((volatile STM32_SYSCFG_REG_TYPE*)0x40013800))

