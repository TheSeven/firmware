#pragma once

struct __attribute__((packed)) STM32_USART_REG_TYPE
{
    union CR1
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t UE : 1;
            uint32_t : 1;
            uint32_t RE : 1;
            uint32_t TE : 1;
            uint32_t IDLEIE : 1;
            uint32_t RXNEIE : 1;
            uint32_t TCIE : 1;
            uint32_t TXEIE : 1;
            uint32_t PEIE : 1;
            uint32_t PS : 1;
            uint32_t PCE : 1;
            uint32_t WAKE : 1;
            uint32_t M0 : 1;
            uint32_t MME : 1;
            uint32_t CMIE : 1;
            uint32_t OVER8 : 1;
            uint32_t DEDT : 5;
            uint32_t DEAT : 5;
            uint32_t RTOIE : 1;
            uint32_t EOBIE : 1;
            uint32_t M1 : 1;
            uint32_t : 3;
        } b;
    } CR1;
    union CR2
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t : 4;
            uint32_t ADDM7 : 1;
            uint32_t : 3;
            uint32_t LBCL : 1;
            uint32_t CPHA : 1;
            uint32_t CPOL : 1;
            uint32_t CLKEN : 1;
            uint32_t STOP : 2;
            uint32_t : 1;
            uint32_t SWAP : 1;
            uint32_t RXINV : 1;
            uint32_t TXINV : 1;
            uint32_t DATAINV : 1;
            uint32_t MSBFIRST : 1;
            uint32_t ABREN : 1;
            uint32_t ABRMOD : 2;
            uint32_t RTOEN : 1;
            uint32_t ADD : 8;
        } b;
    } CR2;
    union CR3
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t EIE : 1;
            uint32_t : 2;
            uint32_t HDSEL : 1;
            uint32_t : 2;
            uint32_t DMAR : 1;
            uint32_t DMAT : 1;
            uint32_t RTSE : 1;
            uint32_t CTSE : 1;
            uint32_t CTSIE : 1;
            uint32_t ONEBIT : 1;
            uint32_t OVRDIS : 1;
            uint32_t DDRE : 1;
            uint32_t DEM : 1;
            uint32_t DEP : 1;
            uint32_t : 16;
        } b;
    } CR3;
    union BRR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t DIV_Fraction : 4;
            uint32_t DIV_Mantissa : 12;
            uint32_t : 16;
        } b;
    } BRR;
    union GTPR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t : 32;
        } b;
    } GTPR;
    uint32_t RTOR;
    union RQR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t ABRRQ : 1;
            uint32_t SBKRQ : 1;
            uint32_t MMRQ : 1;
            uint32_t RXFRQ : 1;
            uint32_t : 28;
        } b;
    } RQR;
    union ISR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t PE : 1;
            uint32_t FE : 1;
            uint32_t NF : 1;
            uint32_t ORE : 1;
            uint32_t IDLE : 1;
            uint32_t RXNE : 1;
            uint32_t TC : 1;
            uint32_t TXE : 1;
            uint32_t : 1;
            uint32_t CTSIF : 1;
            uint32_t CTS : 1;
            uint32_t RTOF : 1;
            uint32_t : 2;
            uint32_t ABRE : 1;
            uint32_t ABRF : 1;
            uint32_t BUSY : 1;
            uint32_t CMF : 1;
            uint32_t SBKF : 1;
            uint32_t : 2;
            uint32_t TEACK : 1;
            uint32_t : 10;
        } b;
    } ISR;
    union ICR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t PE : 1;
            uint32_t FE : 1;
            uint32_t NF : 1;
            uint32_t ORE : 1;
            uint32_t IDLE : 1;
            uint32_t : 1;
            uint32_t TC : 1;
            uint32_t : 2;
            uint32_t CTS : 1;
            uint32_t : 1;
            uint32_t RTOF : 1;
            uint32_t : 5;
            uint32_t CMF : 1;
            uint32_t : 14;
        } b;
    } ICR;
    uint32_t RDR;
    uint32_t TDR;
};

#if defined(SOC_STM32F0) || defined(SOC_STM32F1)
#define STM32_USART1_REGS (*((volatile STM32_USART_REG_TYPE*)0x40013800))
#define STM32_USART2_REGS (*((volatile STM32_USART_REG_TYPE*)0x40004400))
#define STM32_USART3_REGS (*((volatile STM32_USART_REG_TYPE*)0x40004800))
#define STM32_UART4_REGS (*((volatile STM32_USART_REG_TYPE*)0x40004c00))
#define STM32_UART5_REGS (*((volatile STM32_USART_REG_TYPE*)0x40005000))
#ifdef SOC_STM32F0
#define STM32_USART6_REGS (*((volatile STM32_USART_REG_TYPE*)0x40011400))
#endif
#else
#define STM32_USART1_REGS (*((volatile STM32_USART_REG_TYPE*)0x40011000))
#define STM32_USART2_REGS (*((volatile STM32_USART_REG_TYPE*)0x40004400))
#define STM32_USART3_REGS (*((volatile STM32_USART_REG_TYPE*)0x40004800))
#define STM32_UART4_REGS (*((volatile STM32_USART_REG_TYPE*)0x40004c00))
#define STM32_UART5_REGS (*((volatile STM32_USART_REG_TYPE*)0x40005000))
#define STM32_USART6_REGS (*((volatile STM32_USART_REG_TYPE*)0x40011400))
#ifdef SOC_STM32F42X
#define STM32_UART7_REGS (*((volatile STM32_USART_REG_TYPE*)0x40007800))
#define STM32_UART8_REGS (*((volatile STM32_USART_REG_TYPE*)0x40007c00))
#endif
#endif
