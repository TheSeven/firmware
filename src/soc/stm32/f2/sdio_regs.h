#pragma once

struct __attribute__((packed)) STM32_SDIO_REG_TYPE
{
    union POWER
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t PWRCTRL : 2;
            uint32_t : 30;
        } b;
    } POWER;
    union CLKCR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t CLKDIV : 8;
            uint32_t CLKEN : 1;
            uint32_t PWRSAV : 1;
            uint32_t BYPASS : 1;
            uint32_t WIDBUS : 2;
            uint32_t NEGEDGE : 1;
            uint32_t HWFC_EN : 1;
            uint32_t : 17;
        } b;
    } CLKCR;
    uint32_t ARG;
    union CMD
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t CMDINDEX : 6;
            uint32_t WAITRESP : 2;
            uint32_t WAITINT : 1;
            uint32_t WAITPEND : 1;
            uint32_t CPSMEN : 1;
            uint32_t SDIOSUSPEND : 1;
            uint32_t ENCMDCOMPL : 1;
            uint32_t NIEN : 1;
            uint32_t CEATACMD : 1;
            uint32_t : 17;
        } b;
    } CMD;
    union RESPCMD
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t RESPCMD : 6;
            uint32_t : 26;
        } b;
    } RESPCMD;
    uint32_t RESP[4];
    uint32_t DTIMER;
    union DLEN
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t DATALENGTH : 25;
            uint32_t : 7;
        } b;
    } DLEN;
    union DCTRL
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t DTEN : 1;
            uint32_t DTDIR : 1;
            uint32_t DTMODE : 1;
            uint32_t DMAEN : 1;
            uint32_t DBLOCKSIZE : 4;
            uint32_t RWSTART : 1;
            uint32_t RWSTOP : 1;
            uint32_t RWMOD : 1;
            uint32_t SDIOEN : 1;
            uint32_t : 20;
        } b;
    } DCTRL;
    union DCOUNT
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t DATACOUNT : 25;
            uint32_t : 7;
        } b;
    } DCOUNT;
    union STA
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t CCRCFAIL : 1;
            uint32_t DCRCFAIL : 1;
            uint32_t CTIMEOUT : 1;
            uint32_t DTIMEOUT : 1;
            uint32_t TXUNDERR : 1;
            uint32_t RXOVERR : 1;
            uint32_t CMDREND : 1;
            uint32_t CMDSENT : 1;
            uint32_t DATAEND : 1;
            uint32_t STBITERR : 1;
            uint32_t DBCKEND : 1;
            uint32_t CMDACT : 1;
            uint32_t TXACT : 1;
            uint32_t RXACT : 1;
            uint32_t TXFIFOHE : 1;
            uint32_t RXFIFOHF : 1;
            uint32_t TXFIFOF : 1;
            uint32_t RXFIFOF : 1;
            uint32_t TXFIFOE : 1;
            uint32_t RXFIFOE : 1;
            uint32_t TXDAVL : 1;
            uint32_t RXDAVL : 1;
            uint32_t SDIOIT : 1;
            uint32_t CEATAEND : 1;
            uint32_t : 8;
        } b;
    } STA;
    union ICR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t CCRCFAILC : 1;
            uint32_t DCRCFAILC : 1;
            uint32_t CTIMEOUTC : 1;
            uint32_t DTIMEOUTC : 1;
            uint32_t TXUNDERRC : 1;
            uint32_t RXOVERRC : 1;
            uint32_t CMDRENDC : 1;
            uint32_t CMDSENTC : 1;
            uint32_t DATAENDC : 1;
            uint32_t STBITERRC : 1;
            uint32_t DBCKENDC : 1;
            uint32_t : 11;
            uint32_t SDIOITC : 1;
            uint32_t CEATAENDC : 1;
            uint32_t : 8;
        } b;
    } ICR;
    union MASK
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t CCRCFAILIE : 1;
            uint32_t DCRCFAILIE : 1;
            uint32_t CTIMEOUTIE : 1;
            uint32_t DTIMEOUTIE : 1;
            uint32_t TXUNDERRIE : 1;
            uint32_t RXOVERRIE : 1;
            uint32_t CMDRENDIE : 1;
            uint32_t CMDSENTIE : 1;
            uint32_t DATAENDIE : 1;
            uint32_t STBITERRIE : 1;
            uint32_t DBCKENDIE : 1;
            uint32_t CMDACTIE : 1;
            uint32_t TXACTIE : 1;
            uint32_t RXACTIE : 1;
            uint32_t TXFIFOHEIE : 1;
            uint32_t RXFIFOHFIE : 1;
            uint32_t TXFIFOFIE : 1;
            uint32_t RXFIFOFIE : 1;
            uint32_t TXFIFOEIE : 1;
            uint32_t RXFIFOEIE : 1;
            uint32_t TXDAVLIE : 1;
            uint32_t RXDAVLIE : 1;
            uint32_t SDIOITIE : 1;
            uint32_t CEATAENDIE : 1;
            uint32_t : 8;
        } b;
    } MASK;
    uint32_t RESERVED0[2];
    union FIFOCNT
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t FIFOCOUNT : 24;
            uint32_t : 8;
        } b;
    } FIFOCNT;
    uint32_t RESERVED1[13];
    uint32_t FIFO[32];
};

#define STM32_SDIO_REGS (*((volatile STM32_SDIO_REG_TYPE*)0x40012c00))
