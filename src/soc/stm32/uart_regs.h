#pragma once

struct __attribute__((packed)) STM32_USART_REG_TYPE
{
    union SR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t PE : 1;
            uint32_t FE : 1;
            uint32_t NE : 1;
            uint32_t ORE : 1;
            uint32_t IDLE : 1;
            uint32_t RXNE : 1;
            uint32_t TC : 1;
            uint32_t TXE : 1;
            uint32_t LBD : 1;
            uint32_t CTS : 1;
            uint32_t : 22;
        } b;
    } SR;
    uint32_t DR;
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
    union CR1
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t SBK : 1;
            uint32_t RWU : 1;
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
            uint32_t M : 1;
            uint32_t UE : 1;
            uint32_t : 1;
            uint32_t OVER8 : 1;
            uint32_t : 16;
        } b;
    } CR1;
    union CR2
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t ADD : 4;
            uint32_t : 1;
            uint32_t LBDL : 1;
            uint32_t LBDIE : 1;
            uint32_t : 1;
            uint32_t LBCL : 1;
            uint32_t CPHA : 1;
            uint32_t CPOL : 1;
            uint32_t CLKEN : 1;
            uint32_t STOP : 2;
            uint32_t LINEN : 1;
            uint32_t : 17;
        } b;
    } CR2;
    union CR3
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t EIE : 1;
            uint32_t IREN : 1;
            uint32_t IRLP : 1;
            uint32_t HDSEL : 1;
            uint32_t NACK : 1;
            uint32_t SCEN : 1;
            uint32_t DMAR : 1;
            uint32_t DMAT : 1;
            uint32_t RTSE : 1;
            uint32_t CTSE : 1;
            uint32_t CTSIE : 1;
            uint32_t ONEBIT : 1;
            uint32_t : 20;
        } b;
    } CR3;
    union GTPR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t PSC : 8;
            uint32_t GT : 8;
            uint32_t : 16;
        } b;
    } GTPR;
};

#ifdef SOC_STM32F1
#define STM32_USART1_REGS (*((volatile STM32_USART_REG_TYPE*)0x40013800))
#else
#define STM32_USART1_REGS (*((volatile STM32_USART_REG_TYPE*)0x40011000))
#endif
#define STM32_USART2_REGS (*((volatile STM32_USART_REG_TYPE*)0x40004400))
#define STM32_USART3_REGS (*((volatile STM32_USART_REG_TYPE*)0x40004800))
#define STM32_UART4_REGS (*((volatile STM32_USART_REG_TYPE*)0x40004c00))
#define STM32_UART5_REGS (*((volatile STM32_USART_REG_TYPE*)0x40005000))
#define STM32_USART6_REGS (*((volatile STM32_USART_REG_TYPE*)0x40011400))
#define STM32_UART7_REGS (*((volatile STM32_USART_REG_TYPE*)0x40007800))
#define STM32_UART8_REGS (*((volatile STM32_USART_REG_TYPE*)0x40007c00))
