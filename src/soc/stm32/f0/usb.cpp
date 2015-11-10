#include "global.h"
#include "soc/stm32/f0/usb.h"
#include "soc/stm32/f0/crs_regs.h"
#include "cpu/arm/cortexm/irq.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"
#include "sys/time.h"


STM32::USB* STM32::USB::activeInstance;


void STM32::USB::drvStart()
{
    // Enable clock gate
    clockgate_enable(STM32_USB_CLOCKGATE, true);
    clockgate_enable(STM32_CRS_CLOCKGATE, true);

    // Enable clock recovery system
    union STM32_CRS_REG_TYPE::CR CR = { 0 };
    CR.b.CEN = true;
    CR.b.AUTOTRIMEN = true;
    CR.b.TRIM = 32;
    STM32_CRS_REGS.CR.d32 = CR.d32;

    // Power up USB core and assert reset, enter suspend mode and unmask the IRQ sources that we're interested in
    union STM32_USB_REG_TYPE::CNTR CNTR = { 0 };
    CNTR.b.FRES = true;
    CNTR.b.LP_MODE = true;
    CNTR.b.FSUSP = true;
    CNTR.b.CTRM = true;
    CNTR.b.WKUPM = true;
    CNTR.b.SUSPM = true;
    CNTR.b.RESETM = true;
    STM32_USB_REGS.CNTR.d32 = CNTR.d32;

    // Wait for reference voltages to stabilize
    udelay(2);

    // Deassert reset, stay in suspend mode
    CNTR.b.FRES = false;
    STM32_USB_REGS.CNTR.d32 = CNTR.d32;

    // Set up packet buffer structure
    for (int i = 0; i < 8; i++)
    {
        STM32_USB_BDT[i].ADDR_RX_DT1 = i * 128;
        STM32_USB_BDT[i].ADDR_TX_DT0 = i * 128 + 64;
    }
    STM32_USB_BDT[0].ADDR_RX_DT1 = 64;
    STM32_USB_REGS.BTABLE = 0;

    // Enable link power management
    union STM32_USB_REG_TYPE::LPMCSR LPMCSR = { 0 };
    LPMCSR.b.LPMEN = true;
    STM32_USB_REGS.LPMCSR.d32 = LPMCSR.d32;

    // Clear any leftover IRQs and enable our IRQ handler
    union STM32_USB_REG_TYPE::ISTR ISTR = { 0 };
    STM32_USB_REGS.ISTR.d32 = ISTR.d32;
    activeInstance = this;
    irq_clear_pending(usb_IRQn);
    irq_enable(usb_IRQn, true);

    // Soft connect
    CR.d32 = STM32_CRS_REGS.CR.d32;
    union STM32_USB_REG_TYPE::BCDR BCDR = { 0 };
    BCDR.b.DPPU = true;
    STM32_USB_REGS.BCDR.d32 = BCDR.d32;
}

void STM32::USB::drvStop()
{
    // Soft disconnect
    union STM32_USB_REG_TYPE::BCDR BCDR = { 0 };
    STM32_USB_REGS.BCDR.d32 = BCDR.d32;

    // Shut down USB core
    union STM32_USB_REG_TYPE::CNTR CNTR = { 0 };
    CNTR.b.FRES = true;
    CNTR.b.PWDN = true;
    STM32_USB_REGS.CNTR.d32 = CNTR.d32;

    // Disable IRQ and clock gate
    irq_enable(usb_IRQn, false);
    clockgate_enable(STM32_CRS_CLOCKGATE, false);
    clockgate_enable(STM32_USB_CLOCKGATE, false);
}

void STM32::USB::copyToPMA(void* dest, const void* src, int len)
{
    uint16_t* d = (uint16_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    len = (len + 1) / 2;
    while (len--)
    {
        uint8_t data = *s++;
        *d++ = data | (*s++ << 8);
    }
}

void STM32::USB::copyFromPMA(void* dest, const void* src, int len)
{
    uint8_t* d = (uint8_t*)dest;
    const uint16_t* s = (const uint16_t*)src;
    while (len-- > 0)
    {
        uint16_t data = *s++;
        *d++ = data;
        if (len--) *d++ = data >> 8;
    }
}

void STM32::USB::setRxSize(::USB::EndpointNumber ep, int size)
{
    STM32_USB_BD_TYPE::COUNT COUNT = { 0 };
    if (size > 31)
    {
        COUNT.b.BLSIZE = true;
        COUNT.b.NUM_BLOCK = MIN(2, (size + 31) >> 5);
    }
    else COUNT.b.NUM_BLOCK = size;
    STM32_USB_BDT[ep.number].COUNT_RX_DT1.d16 = COUNT.d16;
}

void STM32::USB::drvEp0StartRx(bool nonSetup, int len)
{
    // Set up packet size
    if (!nonSetup) len = 8;
    setRxSize(::USB::EndpointNumber(::USB::In, 0), len);

    // Set up endpoint
    union STM32_USB_REG_TYPE::EPR EPR = { 0 };
    union STM32_USB_REG_TYPE::EPR oldEPR = { STM32_USB_REGS.EPR[0].d32 };
    EPR.b.EP_TYPE = (typeof(EPR.b.EP_TYPE))(oldEPR.b.EP_TYPE ^ EPR.b.EP_TYPE_CONTROL);
    if (!nonSetup) EPR.b.STAT_RX = (typeof(EPR.b.STAT_RX))(oldEPR.b.STAT_RX ^ EPR.b.STAT_NAK);
    else EPR.b.STAT_RX = (typeof(EPR.b.STAT_RX))(oldEPR.b.STAT_RX ^ EPR.b.STAT_VALID);
    STM32_USB_REGS.EPR[0].d32 = EPR.d32;
}

void STM32::USB::drvEp0StartTx(const void* buf, int len)
{
    // Copy buffer
    copyToPMA(const_cast<uint32_t*>(&STM32_USB_RAM[0x10]), buf, len);

    // Set up packet size
    STM32_USB_BD_TYPE::COUNT COUNT = { 0 };
    COUNT.b.COUNT = len;
    STM32_USB_BDT[0].COUNT_TX_DT0.d16 = COUNT.d16;

    // Set up endpoint
    union STM32_USB_REG_TYPE::EPR EPR = { 0 };
    union STM32_USB_REG_TYPE::EPR oldEPR = { STM32_USB_REGS.EPR[0].d32 };
    EPR.b.EP_TYPE = (typeof(EPR.b.EP_TYPE))(oldEPR.b.EP_TYPE ^ EPR.b.EP_TYPE_CONTROL);
    if (!len) EPR.b.EP_KIND = true;
    EPR.b.STAT_TX = (typeof(EPR.b.STAT_RX))(oldEPR.b.STAT_TX ^ EPR.b.STAT_VALID);
    STM32_USB_REGS.EPR[0].d32 = EPR.d32;
}

int STM32::USB::drvGetStall(::USB::EndpointNumber ep)
{
    union STM32_USB_REG_TYPE::EPR EPR = { STM32_USB_REGS.EPR[ep.number].d32 };
    return ep.direction == ::USB::Out ? EPR.b.STAT_RX == EPR.b.STAT_STALL : EPR.b.STAT_TX == EPR.b.STAT_STALL;
}

void STM32::USB::setAddress(uint8_t address)
{
    pendingAddress = 0;
    union STM32_USB_REG_TYPE::DADDR DADDR = { 0 };
    DADDR.b.ADD = address;
    DADDR.b.EF = true;
    STM32_USB_REGS.DADDR.d32 = DADDR.d32;
}

void STM32::USB::drvSetAddress(uint8_t address)
{
    // If we want to return to address 0, do it immediately
    if (!address) setAddress(0);
    // Otherwise wait until the next packet transmission is completed (which acknowledges the address change)
    else pendingAddress = address;
}

void STM32::USB::startRx(::USB::EndpointNumber ep, void* buf, int size)
{
    // Store buffer information
    EndpointState* state = &epState[ep.number - 1];
    state->rxAddr = buf;
    state->rxBytes = size;

    // Set up packet size
    setRxSize(ep, size);

    // Set up endpoint
    union STM32_USB_REG_TYPE::EPR oldEPR = { STM32_USB_REGS.EPR[ep.number].d32 };
    union STM32_USB_REG_TYPE::EPR EPR = { 0 };
    EPR.b.EA = oldEPR.b.EA;
    EPR.b.EP_TYPE = oldEPR.b.EP_TYPE;
    EPR.b.STAT_RX = (typeof(EPR.b.STAT_RX))(oldEPR.b.STAT_RX ^ EPR.b.STAT_VALID);
    STM32_USB_REGS.EPR[ep.number].d32 = EPR.d32;
}

bool STM32::USB::pushData(::USB::EndpointNumber ep)
{
    EndpointState* state = &epState[ep.number - 1];
    if (!state->txBytes) return false;
    int size = MIN(state->txBytes, 64);
    copyToPMA(const_cast<uint32_t*>(&STM32_USB_RAM[ep.number * 0x20 + 0x10]), state->txAddr, size);
    state->txAddr = (void*)(((int)state->txAddr) + size);
    state->txBytes -= size;
    STM32_USB_BD_TYPE::COUNT COUNT = { 0 };
    COUNT.b.COUNT = size;
    STM32_USB_BDT[ep.number].COUNT_TX_DT0.d16 = COUNT.d16;
    return true;
}

void STM32::USB::startTx(::USB::EndpointNumber ep, const void* buf, int size)
{
    // Store buffer information
    EndpointState* state = &epState[ep.number - 1];
    state->txAddr = buf;
    state->txBytes = size;

    // Copy first chunk to packet Buffer
    pushData(ep);

    // Set up endpoint
    union STM32_USB_REG_TYPE::EPR oldEPR = { STM32_USB_REGS.EPR[ep.number].d32 };
    union STM32_USB_REG_TYPE::EPR EPR = { 0 };
    EPR.b.EA = oldEPR.b.EA;
    EPR.b.EP_TYPE = oldEPR.b.EP_TYPE;
    EPR.b.STAT_TX = (typeof(EPR.b.STAT_RX))(oldEPR.b.STAT_TX ^ EPR.b.STAT_VALID);
    STM32_USB_REGS.EPR[ep.number].d32 = EPR.d32;
}

void STM32::USB::setStall(::USB::EndpointNumber ep, bool stall)
{
    union STM32_USB_REG_TYPE::EPR oldEPR = { STM32_USB_REGS.EPR[0].d32 };
    union STM32_USB_REG_TYPE::EPR EPR = { 0 };
    EPR.b.EA = oldEPR.b.EA;
    EPR.b.EP_TYPE = oldEPR.b.EP_TYPE;
    typeof(STM32_USB_REGS.EPR->b.STAT_RX) value = stall ? EPR.b.STAT_STALL : EPR.b.STAT_NAK;
    if (ep.direction == ::USB::Out) EPR.b.STAT_RX = (typeof(EPR.b.STAT_RX))(oldEPR.b.STAT_RX ^ value);
    else EPR.b.STAT_TX = (typeof(EPR.b.STAT_TX))(oldEPR.b.STAT_TX ^ value);
    if (!stall)
    {
        if (ep.direction == ::USB::Out) EPR.b.DTOG_RX = oldEPR.b.DTOG_RX;
        else EPR.b.DTOG_TX = oldEPR.b.DTOG_TX;
    }
    STM32_USB_REGS.EPR[0].d32 = EPR.d32;
}

void STM32::USB::configureEp(::USB::EndpointNumber ep, ::USB::EndpointType type, int maxPacket)
{
    union STM32_USB_REG_TYPE::EPR oldEPR = { STM32_USB_REGS.EPR[ep.number].d32 };
    union STM32_USB_REG_TYPE::EPR EPR = { 0 };
    EPR.b.EA = ep.number;
    EPR.b.STAT_TX = EPR.b.STAT_NAK;
    EPR.b.STAT_RX = EPR.b.STAT_NAK;
    switch (type)
    {
    case ::USB::Bulk: EPR.b.EP_TYPE = EPR.b.EP_TYPE_BULK; break;
    case ::USB::Control: EPR.b.EP_TYPE = EPR.b.EP_TYPE_CONTROL; break;
    case ::USB::Interrupt: EPR.b.EP_TYPE = EPR.b.EP_TYPE_INTERRUPT; break;
    case ::USB::Isochronous: EPR.b.EP_TYPE = EPR.b.EP_TYPE_ISO; break;
    }
    if (ep.direction == ::USB::Out) EPR.b.DTOG_RX = oldEPR.b.DTOG_RX;
    else EPR.b.DTOG_TX = oldEPR.b.DTOG_TX;
    STM32_USB_REGS.EPR[ep.number].d32 = STM32_USB_REGS.EPR[ep.number].d32;
    STM32_USB_REGS.EPR[ep.number].d32 = EPR.d32;
}

void STM32::USB::unconfigureEp(::USB::EndpointNumber ep)
{
    union STM32_USB_REG_TYPE::EPR EPR = { 0 };
    EPR.b.EA = ep.number;
    STM32_USB_REGS.EPR[ep.number].d32 = STM32_USB_REGS.EPR[ep.number].d32;
    STM32_USB_REGS.EPR[ep.number].d32 = EPR.d32;
}

int STM32::USB::getMaxTransferSize(::USB::EndpointNumber ep)
{
    return 1023;  // Limited by 16 bit "bytes left" counter
}

void STM32::USB::handleIrqInternal()
{
    union STM32_USB_REG_TYPE::ISTR ISTR = { STM32_USB_REGS.ISTR.d32 };

    if (ISTR.b.SUSP)
    {
        // Enter low power mode
        union STM32_USB_REG_TYPE::CNTR CNTR = { STM32_USB_REGS.CNTR.d32 };
        CNTR.b.FSUSP = true;
        STM32_USB_REGS.CNTR.d32 = CNTR.d32;
        CNTR.b.LP_MODE = true;
        STM32_USB_REGS.CNTR.d32 = CNTR.d32;
    }

    if (ISTR.b.WKUP || ISTR.b.RESET)
    {
        // Exit suspend mode
        union STM32_USB_REG_TYPE::CNTR CNTR = { STM32_USB_REGS.CNTR.d32 };
        CNTR.b.FSUSP = false;
        STM32_USB_REGS.CNTR.d32 = CNTR.d32;
    }

    if (ISTR.b.RESET)
    {
        // Set device address to 0 and enable packet processing
        setAddress(0);
        // Notify USB core
        handleBusReset(false);
    }

    if (ISTR.b.CTR)
    {
        ::USB::EndpointNumber ep(ISTR.b.DIR ? ::USB::Out : ::USB::In, ISTR.b.EP_ID);
        union STM32_USB_REG_TYPE::EPR oldEPR = { STM32_USB_REGS.EPR[ep.number].d32 };
        union STM32_USB_REG_TYPE::EPR EPR = { 0 };
        EPR.b.EA = oldEPR.b.EA;
        EPR.b.EP_TYPE = oldEPR.b.EP_TYPE;
        EPR.b.CTR_RX = true;
        EPR.b.CTR_TX = true;
        if (ep.direction == ::USB::Out)
        {
            int len = STM32_USB_BDT[ep.number].COUNT_RX_DT1.b.COUNT;
            EPR.b.CTR_RX = false;
            if (oldEPR.b.SETUP)
            {
                EPR.b.STAT_RX = (typeof(EPR.b.STAT_RX))(oldEPR.b.STAT_RX ^ EPR.b.STAT_NAK);
                EPR.b.STAT_TX = (typeof(EPR.b.STAT_TX))(oldEPR.b.STAT_TX ^ EPR.b.STAT_NAK);
                EPR.b.DTOG_TX = oldEPR.b.DTOG_TX ^ 1;
                EPR.b.DTOG_RX = oldEPR.b.DTOG_RX ^ 1;
            }
            if (ep.number)
            {
                // Copy data from packet buffer memory
                EndpointState* state = &epState[ep.number - 1];
                int size = MIN(state->rxBytes, len);
                copyFromPMA(state->rxAddr, const_cast<uint32_t*>(&STM32_USB_RAM[ep.number * 0x20]), size);
                if (oldEPR.b.SETUP) handleSetupReceived(ep, 0);
                else
                {
                    state->rxAddr = (void*)(((int)state->rxAddr) + size);
                    state->rxBytes -= size;
                    // If we're finished or the host sent a short packet, signal transfer completion
                    if (!state->rxBytes || size < 64) handleXferComplete(ep, state->rxBytes);
                    else
                    {
                        // There's more to be received
                        setRxSize(ep, state->rxBytes);
                        EPR.b.STAT_RX = (typeof(EPR.b.STAT_RX))(EPR.b.STAT_NAK ^ EPR.b.STAT_VALID);
                    }
                }
            }
            else
            {
                copyFromPMA(&buffer, const_cast<uint32_t*>(&STM32_USB_RAM[0x10]), len);
                if (oldEPR.b.SETUP) handleSetupReceived(ep, 0);
                else handleXferComplete(ep, 0);
            }
        }
        else
        {
            EPR.b.CTR_TX = false;
            if (!ep.number)
            {
                // Set new address if this packet was acknowledging an address change
                if (pendingAddress) setAddress(pendingAddress);
                handleXferComplete(ep, 0);
            }
            else if (pushData(ep))
            {
                // There's more to be sent
                EPR.b.STAT_TX = (typeof(EPR.b.STAT_TX))(EPR.b.STAT_NAK ^ EPR.b.STAT_VALID);
            }
            else handleXferComplete(ep, 0);
        }
        STM32_USB_REGS.EPR[ep.number].d32 = EPR.d32;
    }

    // Clear any pending IRQ sources
    STM32_USB_REGS.ISTR.d32 = 0;
}

void STM32::USB::handleIrq()
{
    if (STM32::USB::activeInstance) STM32::USB::activeInstance->handleIrqInternal();
    else irq_enable(usb_IRQn, false);
}

extern "C" void usb_irqhandler()
{
    STM32::USB::handleIrq();
}
