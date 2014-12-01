#pragma once

struct __attribute__((packed)) STM32_SPI_REG_TYPE
{
    union CR1
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t CPHA : 1;
            uint32_t CPOL : 1;
            uint32_t MSTR : 1;
            uint32_t BR : 3;
            uint32_t SPE : 1;
            uint32_t LSBFIRST : 1;
            uint32_t SSI : 1;
            uint32_t SSM : 1;
            uint32_t RXONLY : 1;
            uint32_t DFF : 1;
            uint32_t CRCNEXT : 1;
            uint32_t CRCEN : 1;
            uint32_t BIDIOE : 1;
            uint32_t BIDIMODE : 1;
            uint32_t : 16;
        } b;
    } CR1;
    union CR2
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t RXDMAEN : 1;
            uint32_t TXDMAEN : 1;
            uint32_t SSOE : 1;
            uint32_t NSSP : 1;
            uint32_t FRF : 1;
            uint32_t ERRIE : 1;
            uint32_t RXNEIE : 1;
            uint32_t TXEIE : 1;
            uint32_t DS : 4;
            uint32_t FRXTH : 1;
            uint32_t LDMA_RX : 1;
            uint32_t LDMA_TX : 1;
            uint32_t : 17;
        } b;
    } CR2;
    union SR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t RXNE : 1;
            uint32_t TXE : 1;
            uint32_t CHSIDE : 1;
            uint32_t UDR : 1;
            uint32_t CRCERR : 1;
            uint32_t MODF : 1;
            uint32_t OVR : 1;
            uint32_t BSY : 1;
            uint32_t FRE : 1;
            uint32_t FRLVL : 2;
            uint32_t FTLVL : 2;
            uint32_t : 19;
        } b;
    } SR;
    uint8_t DR;
    uint32_t : 24;
    uint32_t CRCPR;
    uint32_t RXCRC;
    uint32_t TXCRC;
    union I2SCFGR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t CHLEN : 1;
            uint32_t DATLEN : 2;
            uint32_t CKPOL : 1;
            uint32_t I2SSTD : 2;
            uint32_t : 1;
            uint32_t PCMSYNC : 1;
            uint32_t I2SCFG : 2;
            uint32_t I2SE : 1;
            uint32_t I2SMOD : 1;
            uint32_t : 20;
        } b;
    } I2SCFGR;
    union I2SPR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t I2SDIV : 8;
            uint32_t ODD : 1;
            uint32_t MCKOE : 1;
            uint32_t : 22;
        } b;
    } I2SPR;
};

#define STM32_SPI1_REGS (*((volatile STM32_SPI_REG_TYPE*)0x40013000))
#define STM32_SPI2_REGS (*((volatile STM32_SPI_REG_TYPE*)0x40003800))
#define STM32_SPI3_REGS (*((volatile STM32_SPI_REG_TYPE*)0x40003c00))
#define STM32_SPI4_REGS (*((volatile STM32_SPI_REG_TYPE*)0x40013400))
#define STM32_SPI5_REGS (*((volatile STM32_SPI_REG_TYPE*)0x40015000))
#define STM32_SPI6_REGS (*((volatile STM32_SPI_REG_TYPE*)0x40015400))
