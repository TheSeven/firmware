#pragma once
#include "global.h"
#include "soc/stm32/f0/i2c.h"

struct __attribute__((packed)) STM32_I2C_REG_TYPE
{
    union CR1
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t PE : 1;
            uint32_t TXIE : 1;
            uint32_t RXIE : 1;
            uint32_t ADDRIE : 1;
            uint32_t NACKIE : 1;
            uint32_t STOPIE : 1;
            uint32_t TCIE : 1;
            uint32_t ERRIE  : 1;
            uint32_t DNF : 4;
            uint32_t ANFOFF : 1;
            uint32_t  : 1;
            uint32_t TXDMAEN : 1;
            uint32_t RXDMAEN : 1;
            uint32_t SBC : 1;
            uint32_t NOSTRETCH : 1;
            uint32_t : 1;
            uint32_t GCEN : 1;
            uint32_t SMBHEN : 1;
            uint32_t SMBDEN : 1;
            uint32_t ALERTEN : 1;
            uint32_t PECEN : 1;
            uint32_t : 8;
        } b;
    } CR1;
    union CR2
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t SADD : 10;
            uint32_t RD_WRN : 1;
            uint32_t ADD10 : 1;
            uint32_t HEAD10R : 1;
            uint32_t START : 1;
            uint32_t STOP : 1;
            uint32_t NACK : 1;
            uint32_t NBYTES : 8;
            uint32_t RELOAD : 1;
            uint32_t AUTOEND : 1;
            uint32_t PECBYTE : 1;
            uint32_t : 5;
        } b;
    } CR2;
    union OAR1
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t OA : 10;
            uint32_t OA1MODE : 1;
            uint32_t : 4;
            uint32_t OA1EN : 1;
            uint32_t : 16;
        } b;
    } OAR1;
    union OAR2
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t : 1;
            uint32_t OA2 : 6;
            uint32_t OA2MSK : 3;
            uint32_t : 4;
            uint32_t OA1EN : 1;
            uint32_t : 16;
        } b;
    } OAR2;
    union STM32::I2C::Timing TIMINGR;
    union TIMEOUTR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t TIMEOUTA : 12;
            uint32_t TIDLE : 1;
            uint32_t : 2;
            uint32_t TIMOUTEN : 1;
            uint32_t TIMEOUTB : 12;
            uint32_t : 3;
            uint32_t TEXTEN : 1;
        } b;
    } TIMEOUTR;
    union ISR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t TXE : 1;
            uint32_t TXIS : 1;
            uint32_t RXNE : 1;
            uint32_t ADDR : 1;
            uint32_t NAKF : 1;
            uint32_t STOPF : 1;
            uint32_t TC : 1;
            uint32_t TCR : 1;
            uint32_t BERR : 1;
            uint32_t ARLO : 1;
            uint32_t OVR : 1;
            uint32_t PECERR : 1;
            uint32_t TIMEOUT : 1;
            uint32_t ALERT : 1;
            uint32_t : 1;
            uint32_t BUSY : 1;
            uint32_t DIR : 1;
            uint32_t ADDCODE : 7;
            uint32_t : 8;
        } b;
    } ISR;
    union ICR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t : 3;
            uint32_t ADDRCF : 1;
            uint32_t NAKCF : 1;
            uint32_t STOPCF : 1;
            uint32_t : 2;
            uint32_t BERRCF : 1;
            uint32_t ARLOCF : 1;
            uint32_t OVRCF : 1;
            uint32_t PECCF : 1;
            uint32_t TIMEOUTCF : 1;
            uint32_t ALERTCF : 1;
            uint32_t : 18;
        } b;
    } ICR;
    uint32_t PRECR;
    uint32_t RXDR;
    uint32_t TXDR;
};

#define STM32_I2C_REGS(x) (*((volatile STM32_I2C_REG_TYPE*)(0x40005400 + (x) * 0x400)))
#define STM32_I2C1_REGS STM32_I2C_REGS(0)
#define STM32_I2C2_REGS STM32_I2C_REGS(1)
