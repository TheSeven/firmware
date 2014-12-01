#pragma once

struct __attribute__((packed)) STM32_DMA_REG_TYPE
{
    union ISR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t GIF1 : 1;
            uint32_t TCIF1 : 1;
            uint32_t HTIF1 : 1;
            uint32_t TEIF1 : 1;
            uint32_t GIF2 : 1;
            uint32_t TCIF2 : 1;
            uint32_t HTIF2 : 1;
            uint32_t TEIF2 : 1;
            uint32_t GIF3 : 1;
            uint32_t TCIF3 : 1;
            uint32_t HTIF3 : 1;
            uint32_t TEIF3 : 1;
            uint32_t GIF4 : 1;
            uint32_t TCIF4 : 1;
            uint32_t HTIF4 : 1;
            uint32_t TEIF4 : 1;
            uint32_t GIF5 : 1;
            uint32_t TCIF5 : 1;
            uint32_t HTIF5 : 1;
            uint32_t TEIF5 : 1;
            uint32_t : 12;
        } b;
    } ISR;
    union IFCR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t CGIF1 : 1;
            uint32_t CTCIF1 : 1;
            uint32_t CHTIF1 : 1;
            uint32_t CTEIF1 : 1;
            uint32_t CGIF2 : 1;
            uint32_t CTCIF2 : 1;
            uint32_t CHTIF2 : 1;
            uint32_t CTEIF2 : 1;
            uint32_t CGIF3 : 1;
            uint32_t CTCIF3 : 1;
            uint32_t CHTIF3 : 1;
            uint32_t CTEIF3 : 1;
            uint32_t CGIF4 : 1;
            uint32_t CTCIF4 : 1;
            uint32_t CHTIF4 : 1;
            uint32_t CTEIF4 : 1;
            uint32_t CGIF5 : 1;
            uint32_t CTCIF5 : 1;
            uint32_t CHTIF5 : 1;
            uint32_t CTEIF5 : 1;
            uint32_t : 12;
        } b;
    } IFCR;
};

struct __attribute__((packed)) STM32_DMA_STREAM_REG_TYPE
{
    union CCR
    {
        uint32_t d32;
        struct __attribute__((packed)) b
        {
            uint32_t EN : 1;
            uint32_t TCIE : 1;
            uint32_t HTIE : 1;
            uint32_t TEIE : 1;
            uint32_t DIR : 1;
            uint32_t CIRC : 1;
            uint32_t PINC : 1;
            uint32_t MINC : 1;
            uint32_t PSIZE : 2;
            uint32_t MSIZE : 2;
            uint32_t PL : 2;
            uint32_t MEM2MEM : 1;
            uint32_t : 17;
        } b;
    } CCR;
    uint32_t CNDTR;
    volatile void* CPAR;
    void* CMAR;
};

#define STM32_DMA_REGS(c) (*((volatile STM32_DMA_REG_TYPE*)0x40020000))
#define STM32_DMA1_REGS STM32_DMA_REGS(0)

#define STM32_DMA_STREAM_REGS(c, s) (*((volatile STM32_DMA_STREAM_REG_TYPE*)(0x40020008 + (s) * 0x14)))
#define STM32_DMA1_STREAM1_REGS STM32_DMA_STREAM_REGS(0, 0)
#define STM32_DMA1_STREAM2_REGS STM32_DMA_STREAM_REGS(0, 1)
#define STM32_DMA1_STREAM3_REGS STM32_DMA_STREAM_REGS(0, 2)
#define STM32_DMA1_STREAM4_REGS STM32_DMA_STREAM_REGS(0, 3)
#define STM32_DMA1_STREAM5_REGS STM32_DMA_STREAM_REGS(0, 4)
