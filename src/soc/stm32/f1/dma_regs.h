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
            uint32_t GIF6 : 1;
            uint32_t TCIF6 : 1;
            uint32_t HTIF6 : 1;
            uint32_t TEIF6 : 1;
            uint32_t GIF7 : 1;
            uint32_t TCIF7 : 1;
            uint32_t HTIF7 : 1;
            uint32_t TEIF7 : 1;
            uint32_t : 4;
        } b;
    } ISR;
    union IFCR
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
            uint32_t GIF6 : 1;
            uint32_t TCIF6 : 1;
            uint32_t HTIF6 : 1;
            uint32_t TEIF6 : 1;
            uint32_t GIF7 : 1;
            uint32_t TCIF7 : 1;
            uint32_t HTIF7 : 1;
            uint32_t TEIF7 : 1;
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
            uint32_t TCIE : 1;
            uint32_t HTIE : 1;
            uint32_t TEIE : 1;
            uint32_t DIR : 1;
            uint32_t CIRC : 1;
            uint32_t PINC : 1;
            uint32_t MINC : 1;
            uint32_t PSIZE : 2;
            uint32_t MSIZE : 2;
            uint32_t MEM2MEM : 1;
            uint32_t : 17;
        } b;
    } CR;
    uint32_t NDTR;
    uint32_t PAR;
    uint32_t MAR;
};

#define STM32_DMA_REGS(c) (*((volatile STM32_DMA_REG_TYPE*)(0x40020000 + (c) * 0x400)))
#define STM32_DMA_STREAM_REGS(c, s) (*((volatile STM32_DMA_STREAM_REG_TYPE*)(0x40020008 + (c) * 0x400 + (s) * 0x14)))
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
