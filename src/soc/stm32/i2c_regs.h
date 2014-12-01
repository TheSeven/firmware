#pragma once

struct __attribute__((packed)) STM32_I2C_REG_TYPE
{
    union CR1
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t PE : 1;
            uint32_t SMBUS : 1;
            uint32_t : 1;
            uint32_t SMBTYPE : 1;
            uint32_t ENARP : 1;
            uint32_t ENPEC : 1;
            uint32_t ENGC : 1;
            uint32_t NOSTRETCH : 1;
            uint32_t START : 1;
            uint32_t STOP : 1;
            uint32_t ACK : 1;
            uint32_t POS : 1;
            uint32_t PEC : 1;
            uint32_t ALERT : 1;
            uint32_t : 1;
            uint32_t SWRST : 1;
            uint32_t : 16;
        } b;
    } CR1;
    union CR2
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t FREQ : 6;
            uint32_t : 2;
            uint32_t ITERREN : 1;
            uint32_t ITEVTEN : 1;
            uint32_t ITBUFEN : 1;
            uint32_t DMAEN : 1;
            uint32_t LAST : 1;
            uint32_t : 19;
        } b;
    } CR2;
    union OAR1
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t ADD0 : 1;
            uint32_t ADD1_7 : 7;
            uint32_t ADD8_9 : 2;
            uint32_t : 5;
            uint32_t ADDMODE : 1;
            uint32_t : 16;
        } b;
    } OAR1;
    union OAR2
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t ENDUAL : 1;
            uint32_t ADD2 : 7;
            uint32_t : 24;
        } b;
    } OAR2;
    uint32_t DR;
    union SR1
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t SB : 1;
            uint32_t ADDR : 1;
            uint32_t BTF : 1;
            uint32_t ADD10 : 1;
            uint32_t STOPF : 1;
            uint32_t : 1;
            uint32_t RXNE : 1;
            uint32_t TXE : 1;
            uint32_t BERR : 1;
            uint32_t ARLO : 1;
            uint32_t AF : 1;
            uint32_t OVR : 1;
            uint32_t PECERR : 1;
            uint32_t : 1;
            uint32_t TIMEOUT : 1;
            uint32_t SMBALERT : 1;
            uint32_t : 16;
        } b;
    } SR1;
    union SR2
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t MSL : 1;
            uint32_t BUSY : 1;
            uint32_t TRA : 1;
            uint32_t : 1;
            uint32_t GENCALL : 1;
            uint32_t SMBDEFAULT : 1;
            uint32_t SMBHOST : 1;
            uint32_t DUALF : 1;
            uint32_t PEC : 8;
            uint32_t : 16;
        } b;
    } SR2;
    union CCR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t CCR : 12;
            uint32_t : 2;
            uint32_t DUTY : 1;
            uint32_t FS : 1;
            uint32_t : 16;
        } b;
    } CCR;
    uint32_t TRISE;
    union FLTR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t DNF : 4;
            uint32_t ANOFF : 1;
            uint32_t : 27;
        } b;
    } FLTR;
};

#define STM32_I2C_REGS(x) (*((volatile STM32_I2C_REG_TYPE*)(0x40005400 + (x) * 0x400)))
#define STM32_I2C1_REGS STM32_I2C_REGS(0)
#define STM32_I2C2_REGS STM32_I2C_REGS(1)
#define STM32_I2C3_REGS STM32_I2C_REGS(2)
