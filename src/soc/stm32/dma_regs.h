#pragma once

struct __attribute__((packed)) STM32_DMA_REG_TYPE
{
    union ISR
    {
        uint16_t d32XXX[2];
        struct __attribute__((packed)) b
        {
            uint32_t FEIF0 : 1;
            uint32_t : 1;
            uint32_t DMEIF0 : 1;
            uint32_t TEIF0 : 1;
            uint32_t HTIF0 : 1;
            uint32_t TCIF0 : 1;
            uint32_t FEIF1 : 1;
            uint32_t : 1;
            uint32_t DMEIF1 : 1;
            uint32_t TEIF1 : 1;
            uint32_t HTIF1 : 1;
            uint32_t TCIF1 : 1;
            uint32_t : 4;
            uint32_t FEIF2 : 1;
            uint32_t : 1;
            uint32_t DMEIF2 : 1;
            uint32_t TEIF2 : 1;
            uint32_t HTIF2 : 1;
            uint32_t TCIF2 : 1;
            uint32_t FEIF3 : 1;
            uint32_t : 1;
            uint32_t DMEIF3 : 1;
            uint32_t TEIF3 : 1;
            uint32_t HTIF3 : 1;
            uint32_t TCIF3 : 1;
            uint32_t : 4;
            uint32_t FEIF4 : 1;
            uint32_t : 1;
            uint32_t DMEIF4 : 1;
            uint32_t TEIF4 : 1;
            uint32_t HTIF4 : 1;
            uint32_t TCIF4 : 1;
            uint32_t FEIF5 : 1;
            uint32_t : 1;
            uint32_t DMEIF5 : 1;
            uint32_t TEIF5 : 1;
            uint32_t HTIF5 : 1;
            uint32_t TCIF5 : 1;
            uint32_t : 4;
            uint32_t FEIF6 : 1;
            uint32_t : 1;
            uint32_t DMEIF6 : 1;
            uint32_t TEIF6 : 1;
            uint32_t HTIF6 : 1;
            uint32_t TCIF6 : 1;
            uint32_t FEIF7 : 1;
            uint32_t : 1;
            uint32_t DMEIF7 : 1;
            uint32_t TEIF7 : 1;
            uint32_t HTIF7 : 1;
            uint32_t TCIF7 : 1;
            uint32_t : 4;
        } b;
    } ISR;
    union IFCR
    {
        uint32_t d32[2];
        struct __attribute__((packed)) b
        {
            uint32_t CFEIF0 : 1;
            uint32_t : 1;
            uint32_t CDMEIF0 : 1;
            uint32_t CTEIF0 : 1;
            uint32_t CHTIF0 : 1;
            uint32_t CTCIF0 : 1;
            uint32_t CFEIF1 : 1;
            uint32_t : 1;
            uint32_t CDMEIF1 : 1;
            uint32_t CTEIF1 : 1;
            uint32_t CHTIF1 : 1;
            uint32_t CTCIF1 : 1;
            uint32_t : 4;
            uint32_t CFEIF2 : 1;
            uint32_t : 1;
            uint32_t CDMEIF2 : 1;
            uint32_t CTEIF2 : 1;
            uint32_t CHTIF2 : 1;
            uint32_t CTCIF2 : 1;
            uint32_t CFEIF3 : 1;
            uint32_t : 1;
            uint32_t CDMEIF3 : 1;
            uint32_t CTEIF3 : 1;
            uint32_t CHTIF3 : 1;
            uint32_t CTCIF3 : 1;
            uint32_t : 4;
            uint32_t CFEIF4 : 1;
            uint32_t : 1;
            uint32_t CDMEIF4 : 1;
            uint32_t CTEIF4 : 1;
            uint32_t CHTIF4 : 1;
            uint32_t CTCIF4 : 1;
            uint32_t CFEIF5 : 1;
            uint32_t : 1;
            uint32_t CDMEIF5 : 1;
            uint32_t CTEIF5 : 1;
            uint32_t CHTIF5 : 1;
            uint32_t CTCIF5 : 1;
            uint32_t : 4;
            uint32_t CFEIF6 : 1;
            uint32_t : 1;
            uint32_t CDMEIF6 : 1;
            uint32_t CTEIF6 : 1;
            uint32_t CHTIF6 : 1;
            uint32_t CTCIF6 : 1;
            uint32_t CFEIF7 : 1;
            uint32_t : 1;
            uint32_t CDMEIF7 : 1;
            uint32_t CTEIF7 : 1;
            uint32_t CHTIF7 : 1;
            uint32_t CTCIF7 : 1;
            uint32_t : 4;
        } b;
    } IFCR;
};

struct __attribute__((packed)) STM32_DMA_STREAM_REG_TYPE
{
    union CR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t EN : 1;
            uint32_t DMEIE : 1;
            uint32_t TEIE : 1;
            uint32_t HTIE : 1;
            uint32_t TCIE : 1;
            uint32_t PFCTRL : 1;
            uint32_t DIR : 2;
            uint32_t CIRC : 1;
            uint32_t PINC : 1;
            uint32_t MINC : 1;
            uint32_t PSIZE : 2;
            uint32_t MSIZE : 2;
            uint32_t PINCOS : 1;
            uint32_t PL : 2;
            uint32_t DBM : 1;
            uint32_t CT : 1;
            uint32_t ACK : 1;
            uint32_t PBURST : 2;
            uint32_t MBURST : 2;
            uint32_t CHSEL : 3;
            uint32_t : 4;
        } b;
    } CR;
    uint32_t NDTR;
    uint32_t PAR;
    uint32_t M0AR;
    uint32_t M1AR;
    union FCR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t FTH : 2;
            uint32_t DMDIS : 1;
            uint32_t FS : 3;
            uint32_t : 1;
            uint32_t FEIE : 1;
            uint32_t : 24;
        } b;
    } FCR;
};

#define STM32_DMA_REGS(c) (*((volatile STM32_DMA_REG_TYPE*)(0x40026000 + (c) * 0x400)))
#define STM32_DMA_STREAM_REGS(c, s) (*((volatile STM32_DMA_STREAM_REG_TYPE*)(0x40026010 + (c) * 0x400 + (s) * 0x18)))
#define STM32_DMA1_REGS STM32_DMA_REGS(0)
#define STM32_DMA2_REGS STM32_DMA_REGS(1)
#define STM32_DMA1_STREAM0_REGS STM32_DMA_STREAM_REGS(0, 0)
#define STM32_DMA1_STREAM1_REGS STM32_DMA_STREAM_REGS(0, 1)
#define STM32_DMA1_STREAM2_REGS STM32_DMA_STREAM_REGS(0, 2)
#define STM32_DMA1_STREAM3_REGS STM32_DMA_STREAM_REGS(0, 3)
#define STM32_DMA1_STREAM4_REGS STM32_DMA_STREAM_REGS(0, 4)
#define STM32_DMA1_STREAM5_REGS STM32_DMA_STREAM_REGS(0, 5)
#define STM32_DMA1_STREAM6_REGS STM32_DMA_STREAM_REGS(0, 6)
#define STM32_DMA1_STREAM7_REGS STM32_DMA_STREAM_REGS(0, 7)
#define STM32_DMA2_STREAM0_REGS STM32_DMA_STREAM_REGS(1, 0)
#define STM32_DMA2_STREAM1_REGS STM32_DMA_STREAM_REGS(1, 1)
#define STM32_DMA2_STREAM2_REGS STM32_DMA_STREAM_REGS(1, 2)
#define STM32_DMA2_STREAM3_REGS STM32_DMA_STREAM_REGS(1, 3)
#define STM32_DMA2_STREAM4_REGS STM32_DMA_STREAM_REGS(1, 4)
#define STM32_DMA2_STREAM5_REGS STM32_DMA_STREAM_REGS(1, 5)
#define STM32_DMA2_STREAM6_REGS STM32_DMA_STREAM_REGS(1, 6)
#define STM32_DMA2_STREAM7_REGS STM32_DMA_STREAM_REGS(1, 7)
