#pragma once

#include "global.h"
#include "interface/ethernet_mii/ethernet_mii.h"


#ifndef STM32_ETHMAC_BUFFER_SIZE
#define STM32_ETHMAC_BUFFER_SIZE 256
#endif
#ifndef STM32_ETHMAC_NUM_RX_BUFS
#define STM32_ETHMAC_NUM_RX_BUFS 6
#endif
#ifndef STM32_ETHMAC_NUM_TX_BUFS
#define STM32_ETHMAC_NUM_TX_BUFS 6
#endif


namespace STM32
{

    class __attribute__((packed,aligned(4))) ETHMAC final : public Ethernet::MII::MAC
    {
    private:
        struct __attribute__((packed,aligned(4))) DMADescTX
        {
            uint32_t db : 1;
            uint32_t uf : 1;
            uint32_t ed : 1;
            uint32_t cc : 4;
            uint32_t vf : 1;
            uint32_t ec : 1;
            uint32_t lco : 1;
            uint32_t nc : 1;
            uint32_t lca : 1;
            uint32_t ipe : 1;
            uint32_t ff : 1;
            uint32_t jt : 1;
            uint32_t es : 1;
            uint32_t ihe : 1;
            uint32_t ttss : 1;
            uint32_t reserved1 : 2;
            uint32_t tch : 1;
            uint32_t ter : 1;
            uint32_t cic : 2;
            uint32_t reserved2 : 1;
            uint32_t ttse : 1;
            uint32_t dp : 1;
            uint32_t dc : 1;
            uint32_t fs : 1;
            uint32_t ls : 1;
            uint32_t ic : 1;
            uint32_t own : 1;
            uint32_t tbs1 : 13;
            uint32_t reserved3 : 3;
            uint32_t tbs2 : 13;
            uint32_t reserved4 : 3;
            void* tbap1;
            void* tbap2;
            uint64_t reserved5 : 64;
            uint64_t tts : 64;
        };

        struct __attribute__((packed,aligned(4))) DMADescRX
        {
            uint32_t esa : 1;
            uint32_t ce : 1;
            uint32_t dre : 1;
            uint32_t re : 1;
            uint32_t rwt : 1;
            uint32_t ft : 1;
            uint32_t lco : 1;
            uint32_t tsv : 1;
            uint32_t ls : 1;
            uint32_t fs : 1;
            uint32_t vlan : 1;
            uint32_t oe : 1;
            uint32_t le : 1;
            uint32_t saf : 1;
            uint32_t de : 1;
            uint32_t es : 1;
            uint32_t fl : 14;
            uint32_t afm : 1;
            uint32_t own : 1;
            uint32_t rbs1 : 13;
            uint32_t reserved1 : 1;
            uint32_t rch : 1;
            uint32_t rer : 1;
            uint32_t rbs2 : 13;
            uint32_t reserved2 : 2;
            uint32_t dic : 1;
            void* rbap1;
            void* rbap2;
            uint32_t ippt : 3;
            uint32_t iphe : 1;
            uint32_t ippe : 1;
            uint32_t ipcb : 1;
            uint32_t ipv4pr : 1;
            uint32_t ipv6pr : 1;
            uint32_t pmt : 4;
            uint32_t pft : 1;
            uint32_t pv : 1;
            uint64_t reserved3 : 50;
            uint64_t rts : 64;
        };

        struct FrameInfo
        {
            volatile struct DMADescRX *firstSeg;
            volatile uint32_t segCount;
        };

        static volatile struct DMADescTX* curTX;
        static volatile struct DMADescRX* curRX;
        static struct FrameInfo rxFrameInfo;
        static volatile struct FrameInfo* dmaRXFrameInfo;
        static struct DMADescTX txDesc[STM32_ETHMAC_NUM_RX_BUFS] __attribute__((section(".dmabss"),aligned(4)));
        static struct DMADescRX rxDesc[STM32_ETHMAC_NUM_TX_BUFS] __attribute__((section(".dmabss"),aligned(4)));
        static uint8_t txBuf[STM32_ETHMAC_NUM_TX_BUFS][STM32_ETHMAC_BUFFER_SIZE] __attribute__((section(".dmabss"),aligned(4)));
        static uint8_t rxBuf[STM32_ETHMAC_NUM_RX_BUFS][STM32_ETHMAC_BUFFER_SIZE] __attribute__((section(".dmabss"),aligned(4)));
        static Ethernet::Interface* intf;
        static uint8_t macmiiarBase;

    public:
        static void irq();

        ETHMAC(Ethernet::MII::Type type);
        enum Ethernet::Result update(Ethernet::Interface* intf, Ethernet::MII::Driver* driver);
        enum Ethernet::Result phyRead(int phyid, int reg, uint16_t* data);
        enum Ethernet::Result phyWrite(int phyid, int reg, uint16_t data);
        err_t lwipInit(Ethernet::Interface* intf, Ethernet::MII::Driver* driver);
        err_t lwipTx(struct pbuf *p);
    };

}
