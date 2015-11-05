#include "global.h"
#include "soc/stm32/f1/ethmac.h"
#include "soc/stm32/f1/ethmac_regs.h"
#include "cpu/arm/cortexm/irq.h"
#include "interface/irq/irq.h"
#include "interface/clockgate/clockgate.h"
#include "interface/resetline/resetline.h"
#include "sys/time.h"

#ifdef SOC_STM32F1
#include "soc/stm32/f1/rcc.h"
#include "soc/stm32/f1/gpio.h"
#include "soc/stm32/f1/afio.h"
#else
#include "soc/stm32/f2/rcc.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/syscfg.h"
#endif


namespace STM32
{

    volatile struct ETHMAC::DMADescTX* ETHMAC::curTX;
    volatile struct ETHMAC::DMADescRX* ETHMAC::curRX;
    struct ETHMAC::FrameInfo ETHMAC::rxFrameInfo;
    volatile struct ETHMAC::FrameInfo* ETHMAC::dmaRXFrameInfo = &rxFrameInfo;
    struct ETHMAC::DMADescTX ETHMAC::txDesc[STM32_ETHMAC_NUM_RX_BUFS];
    struct ETHMAC::DMADescRX ETHMAC::rxDesc[STM32_ETHMAC_NUM_TX_BUFS];
    uint8_t ETHMAC::txBuf[STM32_ETHMAC_NUM_TX_BUFS][STM32_ETHMAC_BUFFER_SIZE];
    uint8_t ETHMAC::rxBuf[STM32_ETHMAC_NUM_RX_BUFS][STM32_ETHMAC_BUFFER_SIZE];
    Ethernet::Interface* ETHMAC::intf;
    uint8_t ETHMAC::macmiiarBase;

    ETHMAC::ETHMAC(Ethernet::MII::Type type)
    {
        union STM32_ETH_REG_TYPE::MACMIIAR MACMIIAR = { 0 };
        int baseclock = RCC::getAHBClockFrequency();
        if (baseclock > 168000000) MACMIIAR.b.CR = MACMIIAR.b.CR_Div102;
        else if (baseclock > 100000000) MACMIIAR.b.CR = MACMIIAR.b.CR_Div62;
        else if (baseclock > 60000000) MACMIIAR.b.CR = MACMIIAR.b.CR_Div42;
        else if (baseclock > 350000000) MACMIIAR.b.CR = MACMIIAR.b.CR_Div26;
        else MACMIIAR.b.CR = MACMIIAR.b.CR_Div16;
        MACMIIAR.b.MB = true;
        macmiiarBase = (uint8_t)MACMIIAR.d32;
        resetline_assert(STM32_ETHMAC_RESETLINE, true);
#ifdef SOC_STM32F1
        AFIO::setMIIMode(type == Ethernet::MII::TYPE_RMII);
#else
        SYSCFG::setMIIMode(type == Ethernet::MII::TYPE_RMII);
#endif
        clockgate_enable(STM32_ETHMAC_CLOCKGATE, true);
        clockgate_enable(STM32_ETHMACTX_CLOCKGATE, true);
        clockgate_enable(STM32_ETHMACRX_CLOCKGATE, true);
        resetline_assert(STM32_ETHMAC_RESETLINE, false);

        union STM32_ETH_REG_TYPE::DMABMR DMABMR = { 0 };
        DMABMR.b.SR = true;
        STM32_ETH_REGS.DMABMR.d32 = DMABMR.d32;
        while (STM32_ETH_REGS.DMABMR.b.SR);

#ifndef SOC_STM32F1
        clockgate_enable(STM32_ETHMACTX_CLOCKGATE, false);
        clockgate_enable(STM32_ETHMACRX_CLOCKGATE, false);
        clockgate_enable(STM32_ETHMAC_CLOCKGATE, false);
#endif
    }

    enum Ethernet::Result ETHMAC::update(Ethernet::Interface* intf, Ethernet::MII::Driver* driver)
    {
        ETHMAC::intf = intf;
        if (intf->stateChanged)
        {
            if (intf->linkUp)
            {
#ifndef SOC_STM32F1
                clockgate_enable(STM32_ETHMAC_CLOCKGATE, true);
#endif
                union STM32_ETH_REG_TYPE::MACCR MACCR = { 0 };
                MACCR.b.ROD = true;
                MACCR.b.IPCO = true;
                MACCR.b.TE = true;
                MACCR.b.RE = true;
                MACCR.b.FES = driver->speed == Ethernet::MII::SPEED_100;
                MACCR.b.DM = driver->duplex == Ethernet::MII::DUPLEX_FDX;
                STM32_ETH_REGS.MACCR.d32 = MACCR.d32;
#ifndef SOC_STM32F1
                clockgate_enable(STM32_ETHMACTX_CLOCKGATE, true);
                clockgate_enable(STM32_ETHMACRX_CLOCKGATE, true);
#endif
            }
#ifndef SOC_STM32F1
            else
            {
                clockgate_enable(STM32_ETHMACTX_CLOCKGATE, false);
                clockgate_enable(STM32_ETHMACRX_CLOCKGATE, false);
                clockgate_enable(STM32_ETHMAC_CLOCKGATE, false);
            }
#endif
        }
        return Ethernet::RESULT_OK;
    }

#ifdef STM32_BITBANG_MDIO
    static void mdioSendBit(bool bit)
    {
        ::GPIO::setLevel(PIN_A2, bit);
        udelay(1);
        ::GPIO::setLevel(PIN_C1, true);
        udelay(1);
        ::GPIO::setLevel(PIN_C1, false);
    }

    static bool mdioGetBit()
    {
        udelay(1);
        ::GPIO::setLevel(PIN_C1, true);
        udelay(1);
        ::GPIO::setLevel(PIN_C1, false);
        return ::GPIO::getLevel(PIN_A2);
    }

    static void mdioSendBits(uint32_t data, int len)
    {
        while (len--) mdioSendBit((data >> len) & 1);
    }

    static uint32_t mdioGetBits(int len)
    {
        uint16_t data = 0;
        while (len--) data = (data << 1) | mdioGetBit();
        return data;
    }

    static void mdioSendHeader(int opcode, int phyid, int reg)
    {
        ::GPIO::configure(PIN_A2, ::GPIO::MODE_OUTPUT, true);
        mdioSendBits(0xffffffff, 32);
        mdioSendBits((0xd << 12) | (opcode << 10) | (phyid << 5) | reg, 16);
    }

    enum Ethernet::Result ETHMAC::phyRead(int phyid, int reg, uint16_t* data)
    {
        mdioSendHeader(2, phyid, reg);
        ::GPIO::configure(PIN_A2, ::GPIO::MODE_INPUT, true);
        if (mdioGetBit())
        {
            mdioGetBits(32);
            return Ethernet::RESULT_UNKNOWN_ERROR;
        }
        *data = mdioGetBits(16);
        return Ethernet::RESULT_OK;
    }

    enum Ethernet::Result ETHMAC::phyWrite(int phyid, int reg, uint16_t data)
    {
        mdioSendHeader(1, phyid, reg);
        mdioSendBits((2 << 16) | data, 18);
        return Ethernet::RESULT_OK;
    }
#else
    enum Ethernet::Result ETHMAC::phyRead(int phyid, int reg, uint16_t* data)
    {
        bool old = clockgate_enable(STM32_ETHMAC_CLOCKGATE, true);
        union STM32_ETH_REG_TYPE::MACMIIAR MACMIIAR = { macmiiarBase };
        MACMIIAR.b.PA = phyid;
        MACMIIAR.b.MR = reg;
        STM32_ETH_REGS.MACMIIAR.d32 = MACMIIAR.d32;
        while (STM32_ETH_REGS.MACMIIAR.b.MB);
        *data = STM32_ETH_REGS.MACMIIDR.b.MD;
        clockgate_enable(STM32_ETHMAC_CLOCKGATE, old);
        return Ethernet::RESULT_OK;
    }

    enum Ethernet::Result ETHMAC::phyWrite(int phyid, int reg, uint16_t data)
    {
        bool old = clockgate_enable(STM32_ETHMAC_CLOCKGATE, true);
        STM32_ETH_REGS.MACMIIDR.d32 = data;
        union STM32_ETH_REG_TYPE::MACMIIAR MACMIIAR = { macmiiarBase };
        MACMIIAR.b.PA = phyid;
        MACMIIAR.b.MR = reg;
        MACMIIAR.b.MW = true;
        STM32_ETH_REGS.MACMIIAR.d32 = MACMIIAR.d32;
        while (STM32_ETH_REGS.MACMIIAR.b.MB);
        clockgate_enable(STM32_ETHMAC_CLOCKGATE, old);
        return Ethernet::RESULT_OK;
    }
#endif

    err_t ETHMAC::lwipInit(Ethernet::Interface* intf, Ethernet::MII::Driver* driver)
    {
        uint32_t i;

        memset(txDesc, 0, sizeof(txDesc));
        for (i = 0; i < STM32_ETHMAC_NUM_TX_BUFS; i++)
        {
            txDesc[i].tch = true;
            txDesc[i].cic = 3;
            txDesc[i].tbap1 = &txBuf[i];
            if (i < STM32_ETHMAC_NUM_TX_BUFS - 1) txDesc[i].tbap2 = &txDesc[i + 1];
            else txDesc[i].tbap2 = &txDesc[0];
        }

        memset(rxDesc, 0, sizeof(rxDesc));
        for (i = 0; i < STM32_ETHMAC_NUM_RX_BUFS; i++)
        {
            rxDesc[i].rch = true;
            rxDesc[i].rbs1 = STM32_ETHMAC_BUFFER_SIZE;
            rxDesc[i].rbap1 = &rxBuf[i];
            if (i < STM32_ETHMAC_NUM_RX_BUFS - 1) rxDesc[i].rbap2 = &rxDesc[i + 1];
            else rxDesc[i].rbap2 = &rxDesc[0];
            rxDesc[i].own = true;
        }

        curTX = &txDesc[0];
        curRX = &rxDesc[0];

#ifndef SOC_STM32F1
        clockgate_enable(STM32_ETHMAC_CLOCKGATE, true);
#endif

        union STM32_ETH_REG_TYPE::MACCR MACCR = { 0 };
        MACCR.b.FES = true;
        MACCR.b.ROD = true;
        MACCR.b.IPCO = true;
        MACCR.b.DM = true;
        STM32_ETH_REGS.MACCR.d32 = MACCR.d32;

        union STM32_ETH_REG_TYPE::MACFFR MACFFR = { 0 };
        MACFFR.b.PCF = MACFFR.b.PCF_BlockAll;
        STM32_ETH_REGS.MACFFR.d32 = MACFFR.d32;

        union STM32_ETH_REG_TYPE::MACFCR MACFCR = { 0 };
        MACFCR.b.RFCE = true;
        MACFCR.b.TFCE = true;
        MACFCR.b.PT = 0x100;
        STM32_ETH_REGS.MACFCR.d32 = MACFCR.d32;

        union STM32_ETH_REG_TYPE::DMAOMR DMAOMR = { 0 };
        DMAOMR.b.RSF = true;
        DMAOMR.b.TSF = true;
        DMAOMR.b.OSF = true;
        STM32_ETH_REGS.DMAOMR.d32 = DMAOMR.d32;

        union STM32_ETH_REG_TYPE::DMABMR DMABMR = { 0 };
        DMABMR.b.AAB = true;
        DMABMR.b.FB = true;
        DMABMR.b.RTPR = DMABMR.b.RTPR_2_1;
        DMABMR.b.PBL = 2;
        DMABMR.b.EDE = true;
        STM32_ETH_REGS.DMABMR.d32 = DMABMR.d32;

        STM32_ETH_REGS.MACA0HR.d32 = (intf->lwipIf.hwaddr[5] << 8) | intf->lwipIf.hwaddr[4];
        STM32_ETH_REGS.MACA0LR = (intf->lwipIf.hwaddr[3] << 24) | (intf->lwipIf.hwaddr[2] << 16)
                               | (intf->lwipIf.hwaddr[1] << 8) | intf->lwipIf.hwaddr[0];

        STM32_ETH_REGS.DMATDLAR = (uint32_t)txDesc;
        STM32_ETH_REGS.DMARDLAR = (uint32_t)rxDesc;

#ifndef SOC_STM32F1
        clockgate_enable(STM32_ETHMACTX_CLOCKGATE, true);
        clockgate_enable(STM32_ETHMACRX_CLOCKGATE, true);
#endif

        STM32_ETH_REGS.DMAOMR.b.FTF = true;
        while (STM32_ETH_REGS.DMAOMR.b.FTF);

        MACCR.b.TE = true;
        MACCR.b.RE = true;
        STM32_ETH_REGS.MACCR.d32 = MACCR.d32;

        DMAOMR.b.ST = true;
        DMAOMR.b.SR = true;
        STM32_ETH_REGS.DMAOMR.d32 = DMAOMR.d32;

        union STM32_ETH_REG_TYPE::DMAIER DMAIER = { 0 };
        DMAIER.b.NISE = true;
        DMAIER.b.AISE = true;
        DMAIER.b.RBUIE = true;
        DMAIER.b.RIE = true;
        STM32_ETH_REGS.DMAIER.d32 = DMAIER.d32;

#ifndef SOC_STM32F1
        clockgate_enable(STM32_ETHMACTX_CLOCKGATE, false);
        clockgate_enable(STM32_ETHMACRX_CLOCKGATE, false);
        clockgate_enable(STM32_ETHMAC_CLOCKGATE, false);
#endif

        irq_clear_pending(eth_IRQn);
        irq_enable(eth_IRQn, true);

        return ERR_OK;
    }

    err_t ETHMAC::lwipTx(struct pbuf *p)
    {
        volatile struct DMADescTX* desc = curTX;
        struct pbuf* q = p;
        int len = 0;
        int l = 0;
        int o = 0;
        while (q)
        {
            if (desc->own) return ERR_MEM;
            int chunk = MIN(q->len - o, STM32_ETHMAC_BUFFER_SIZE - l);
            memcpy((void*)(((uintptr_t)desc->tbap1) + l), (void*)(((uintptr_t)q->payload) + o), chunk);
            o += chunk;
            if (o >= q->len)
            {
                q = q->next;
                o = 0;
            }
            l += chunk;
            if (l >= STM32_ETHMAC_BUFFER_SIZE)
            {
                desc = (volatile struct DMADescTX*)desc->tbap2;
                l = 0;
            }
            len += chunk;
        }
        bool first = true;
        while (len > 0)
        {
            curTX->fs = first;
            curTX->ls = len <= STM32_ETHMAC_BUFFER_SIZE;
            curTX->tbs1 = MIN(len, STM32_ETHMAC_BUFFER_SIZE);
            curTX->own = true;
            curTX = (volatile struct DMADescTX*)curTX->tbap2;
            len -= STM32_ETHMAC_BUFFER_SIZE;
            first = false;
        }
        if (STM32_ETH_REGS.DMASR.b.TBUS)
        {
            union STM32_ETH_REG_TYPE::DMASR DMASR = { 0 };
            DMASR.b.TBUS = true;
            STM32_ETH_REGS.DMASR.d32 = DMASR.d32;
            STM32_ETH_REGS.DMATPDR = 0;
        }
        return ERR_OK;
    }

    void ETHMAC::irq()
    {
        while (true)
        {
            union STM32_ETH_REG_TYPE::DMASR DMASR = { 0 };
            DMASR.b.NIS = true;
            DMASR.b.AIS = true;
            DMASR.b.RBUS = true;
            DMASR.b.RS = true;
            STM32_ETH_REGS.DMASR.d32 = DMASR.d32;
            if (curRX->own) return;
            if (curRX->fs)
            {
                dmaRXFrameInfo->firstSeg = curRX;
                dmaRXFrameInfo->segCount = 0;
            }
            dmaRXFrameInfo->segCount++;
            if (curRX->ls)
            {
                volatile struct DMADescRX* desc = dmaRXFrameInfo->firstSeg;
                int length = curRX->fl - 4;
                struct pbuf* p = NULL;
                if (!curRX->es && ETHMAC::intf) p = pbuf_alloc(PBUF_RAW, length, PBUF_POOL);
                struct pbuf* q = p;
                int o = 0;
                int l = 0;
                while (dmaRXFrameInfo->segCount)
                {
                    int chunk = MIN(length, STM32_ETHMAC_BUFFER_SIZE - l);
                    if (q)
                    {
                        chunk = MIN(q->len - o, chunk);
                        memcpy((void*)(((uintptr_t)q->payload) + o), (void*)(((uintptr_t)desc->rbap1) + l), chunk);
                        o += chunk;
                        if (o >= q->len)
                        {
                            q = q->next;
                            o = 0;
                        }
                    }
                    l += chunk;
                    length -= chunk;
                    if (l >= STM32_ETHMAC_BUFFER_SIZE || !length)
                    {
                        dmaRXFrameInfo->segCount--;
                        desc->own = true;
                        desc = (volatile struct DMADescRX*)desc->rbap2;
                        l = 0;
                    }
                }
                if (STM32_ETH_REGS.DMASR.b.RBUS)
                {
                    union STM32_ETH_REG_TYPE::DMASR DMASR = { 0 };
                    DMASR.b.RBUS = true;
                    STM32_ETH_REGS.DMASR.d32 = DMASR.d32;
                    STM32_ETH_REGS.DMARPDR = 0;
                }
                if (p && ETHMAC::intf->lwipIf.input(p, &ETHMAC::intf->lwipIf) != ERR_OK) pbuf_free(p);
            }
            curRX = (volatile struct DMADescRX*)curRX->rbap2;
        }
    }

}

extern "C" void eth_irqhandler()
{
    STM32::ETHMAC::irq();
}
