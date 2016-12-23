#include "global.h"
#include "core/dwotg/dwotg.h"
#include "sys/util.h"
#include "sys/time.h"


#ifndef DWOTG_AHB_BURST_LEN
#define DWOTG_AHB_BURST_LEN 5
#endif
#ifndef DWOTG_AHB_THRESHOLD
#define DWOTG_AHB_THRESHOLD 8
#endif
#ifndef DWOTG_TURNAROUND
#define DWOTG_TURNAROUND 3
#endif

void DWOTG::flushOutEndpoint(int ep)
{
    if (!ep) return;
    if (regs->outep_regs[ep].doepctl.b.epena)
    {
        // We are waiting for an OUT packet on this endpoint, which might arrive any moment.
        // Assert a global output NAK to avoid race conditions while shutting down the endpoint.
        socDisableIrq();
        regs->dregs.dctl.b.sgoutnak = 1;
        while (!(regs->gregs.gintsts.b.goutnakeff));
        regs->outep_regs[ep].doepctl.b.snak = 1;
        regs->outep_regs[ep].doepctl.b.epdis = 1;
        while (!(regs->outep_regs[ep].doepint.b.epdisabled));
        regs->dregs.dctl.b.cgoutnak = 1;
        socEnableIrq();
    }
    regs->outep_regs[ep].doepctl.b.usbactep = 0;
    // Reset the transfer size register. Not strictly necessary, but can't hurt.
    regs->outep_regs[ep].doeptsiz.d32 = 0;
}

void DWOTG::flushInEndpoint(int ep)
{
    if (!ep) return;
    if (regs->inep_regs[ep].diepctl.b.epena)
    {
        // We are shutting down an endpoint that might still have IN packets in the FIFO.
        // Disable the endpoint, wait for things to settle, and flush the relevant FIFO.
        socDisableIrq();
        regs->inep_regs[ep].diepctl.b.snak = 1;
        while (!(regs->inep_regs[ep].diepint.b.inepnakeff));
        regs->inep_regs[ep].diepctl.b.epdis = 1;
        while (!(regs->inep_regs[ep].diepint.b.epdisabled));
        regs->inep_regs[ep].diepctl.b.usbactep = 0;
        socEnableIrq();
        // Wait for any DMA activity to stop, to make sure nobody will touch the FIFO.
        while (!regs->gregs.grstctl.b.ahbidle);
        // Flush it all the way down!
        DWOTGRegs::grstctl grstctl = { 0 };
        grstctl.b.txfnum = regs->inep_regs[ep].diepctl.b.txfnum;
        grstctl.b.txfflsh = true;
        regs->gregs.grstctl.d32 = grstctl.d32;
        while (regs->gregs.grstctl.b.txfflsh);
    }
    else if (ep) regs->inep_regs[ep].diepctl.b.usbactep = 0;
    // Reset the transfer size register. Not strictly necessary, but can't hurt.
    regs->inep_regs[ep].dieptsiz.d32 = 0;
}

void DWOTG::flushIrqs()
{
    int i;
    for (i = 0; i < 16; i++)
    {
        regs->outep_regs[i].doepint.d32 = regs->outep_regs[i].doepint.d32;
        regs->inep_regs[i].diepint.d32 = regs->inep_regs[i].diepint.d32;
    }
    regs->gregs.gintsts.d32 = regs->gregs.gintsts.d32;
}

void DWOTG::tryPush(int ep)
{
    DWOTGRegs::depctl depctl = { regs->inep_regs[ep].diepctl.d32 };
    if (!depctl.b.epena || !depctl.b.usbactep || depctl.b.stall || depctl.b.naksts) return;
    int bytesleft = regs->inep_regs[ep].dieptsiz.b.xfersize;
    if (!bytesleft) return;
    int maxpacket = ep ? regs->inep_regs[ep].diepctl.b.mps : 64;
    DWOTGRegs::hnptxsts fifospace = { regs->gregs.hnptxsts.d32 };
    int words = (MIN(maxpacket, bytesleft) + 3) >> 2;
    if (fifospace.b.nptxqspcavail && fifospace.b.nptxfspcavail >= words)
        while (words--) regs->dfifo[ep][0] = *endpoints[ep].txAddr++;
    if (!words && bytesleft <= maxpacket) return;
    regs->gregs.gintmsk.b.nptxfempty = true;
}

void DWOTG::startRx(::USB::EndpointNumber ep, void* buf, int size)
{
    // Find the appropriate set of endpoint registers
    volatile DWOTGRegs::outepregs* epregs = &regs->outep_regs[ep.number];
    // Calculate number of packets (if size == 0 an empty packet will be sent)
    int maxpacket = epregs->doepctl.b.mps;
    int packets = (size + maxpacket - 1) / maxpacket;
    if (!packets) packets = 1;

    // Set up data destination
    if (useDma) epregs->doepdma = buf;
    else endpoints[ep.number].rxAddr = (uint32_t*)buf;
    DWOTGRegs::depxfrsiz deptsiz = { 0 };
    deptsiz.b.pktcnt = packets;
    deptsiz.b.xfersize = size;
    epregs->doeptsiz.d32 = deptsiz.d32;

    // Flush CPU cache if necessary
    if (useDma) invalidate_dcache(buf, size);

    // Enable the endpoint
    DWOTGRegs::depctl depctl = { epregs->doepctl.d32 };
    depctl.b.epena = true;
    depctl.b.cnak = true;
    epregs->doepctl.d32 = depctl.d32;
}

void DWOTG::startTx(::USB::EndpointNumber ep, const void* buf, int size)
{
    // Find the appropriate set of endpoint registers
    volatile DWOTGRegs::inepregs* epregs = &regs->inep_regs[ep.number];
    // Calculate number of packets (if size == 0 an empty packet will be sent)
    int maxpacket = epregs->diepctl.b.mps;
    int packets = (size + maxpacket - 1) / maxpacket;
    if (!packets) packets = 1;

    // Set up data source
    if (useDma) epregs->diepdma = buf;
    else endpoints[ep.number].txAddr = (uint32_t*)buf;
    DWOTGRegs::depxfrsiz deptsiz = { 0 };
    deptsiz.b.pktcnt = packets;
    deptsiz.b.xfersize = size;
    epregs->dieptsiz.d32 = deptsiz.d32;

    // Flush CPU cache if necessary
    if (useDma) clean_dcache(buf, size);

    // Enable the endpoint
    DWOTGRegs::depctl depctl = { epregs->diepctl.d32 };
    depctl.b.epena = true;
    depctl.b.cnak = true;
    epregs->diepctl.d32 = depctl.d32;

    // Start pushing data into the FIFO (must be done after enabling the endpoint)
    if (size && !useDma)
    {
        if (sharedTxFifo) tryPush(ep.number);
        else regs->dregs.diepempmsk.ep.in |= (1 << ep.number);
    }
}

int DWOTG::drvGetStall(::USB::EndpointNumber ep)
{
    if (ep.direction == ::USB::In) return !!regs->inep_regs[ep.number].diepctl.b.stall;
    return !!regs->outep_regs[ep.number].doepctl.b.stall;
}

void DWOTG::setStall(::USB::EndpointNumber ep, bool stall)
{
    if (ep.direction == ::USB::In)
    {
        regs->inep_regs[ep.number].diepctl.b.stall = !!stall;
        if (!stall) regs->inep_regs[ep.number].diepctl.b.setd0pid = true;
    }
    else
    {
        regs->outep_regs[ep.number].doepctl.b.stall = !!stall;
        if (!stall) regs->outep_regs[ep.number].doepctl.b.setd0pid = true;
    }

}

void DWOTG::drvSetAddress(uint8_t address)
{
    regs->dregs.dcfg.b.devaddr = address;
}

void DWOTG::unconfigureEp(::USB::EndpointNumber ep)
{
    if (ep.direction == ::USB::In)
    {
        // Kill any outstanding IN transfers
        flushInEndpoint(ep.number);
        // Mask interrupts for this endpoint
        regs->dregs.daintmsk.ep.in &= ~(1 << ep.number);
    }
    else
    {
        // Kill any outstanding OUT transfers
        flushOutEndpoint(ep.number);
        // Mask interrupts for this endpoint
        regs->dregs.daintmsk.ep.out &= ~(1 << ep.number);
    }
}

void DWOTG::configureEp(::USB::EndpointNumber ep, ::USB::EndpointType type, int maxpacket)
{
    // Kill any outstanding transfers
    unconfigureEp(ep);

    // Write the new configuration and unmask interrupts for the endpoint.
    // Reset data toggle to DATA0, as required by the USB specification.
    int txfifo = sharedTxFifo ? 0 : ep.number;
    DWOTGRegs::depctl depctl = { 0 };
    depctl.b.usbactep = true;
    depctl.b.eptype = type;
    depctl.b.mps = maxpacket;
    depctl.b.txfnum = txfifo;
    depctl.b.setd0pid = true;
    depctl.b.nextep = (ep.number + 1) & 0xf;
    if (ep.direction == ::USB::In)
    {
        regs->inep_regs[ep.number].diepctl.d32 = depctl.d32;
        regs->dregs.daintmsk.ep.in |= (1 << ep.number);
    }
    else
    {
        regs->outep_regs[ep.number].doepctl.d32 = depctl.d32;
        regs->dregs.daintmsk.ep.out |= (1 << ep.number);
    }
}

void DWOTG::drvEp0StartRx(bool nonSetup, int len)
{
    // Set up data destination
    if (useDma) regs->outep_regs[0].doepdma = buffer.u8;
    else endpoints[0].rxAddr = (uint32_t*)buffer.u8;
    DWOTGRegs::dep0xfrsiz deptsiz = { 0 };
    deptsiz.b.supcnt = 3;
    deptsiz.b.pktcnt = !!nonSetup;
    deptsiz.b.xfersize = len;
    regs->outep_regs[0].doeptsiz.d32 = deptsiz.d32;

    // Flush CPU cache if necessary
    if (useDma) invalidate_dcache(buffer.u8, len);

    // Enable the endpoint
    DWOTGRegs::depctl depctl = { regs->outep_regs[0].doepctl.d32 };
    depctl.b.epena = true;
    depctl.b.cnak = !!nonSetup;
    regs->outep_regs[0].doepctl.d32 = depctl.d32;
}

void DWOTG::drvEp0StartTx(const void* buf, int len)
{
    if (len)
    {
        // Set up data source
        if (useDma) regs->inep_regs[0].diepdma = buf;
        else endpoints[0].txAddr = (const uint32_t*)buf;
        DWOTGRegs::dep0xfrsiz deptsiz = { 0 };
        deptsiz.b.pktcnt = (len + 63) >> 6;
        deptsiz.b.xfersize = len;
        regs->inep_regs[0].dieptsiz.d32 = deptsiz.d32;
    }
    else
    {
        // Set up the IN pipe for a zero-length packet
        DWOTGRegs::dep0xfrsiz deptsiz = { 0 };
        deptsiz.b.pktcnt = 1;
        regs->inep_regs[0].dieptsiz.d32 = deptsiz.d32;
    }

    // Flush CPU cache if necessary
    if (useDma) clean_dcache(buf, len);

    // Enable the endpoint
    DWOTGRegs::depctl depctl = { regs->inep_regs[0].diepctl.d32 };
    depctl.b.epena = true;
    depctl.b.cnak = true;
    regs->inep_regs[0].diepctl.d32 = depctl.d32;

    // Start pushing data into the FIFO (must be done after enabling the endpoint)
    if (len && !useDma)
    {
        if (sharedTxFifo) tryPush(0);
        else regs->dregs.diepempmsk.ep.in |= 1;
    }
}

void DWOTG::ep0Init()
{
    // Make sure both EP0 pipes are active.
    // (The hardware should take care of that, but who knows...)
    DWOTGRegs::depctl depctl = { 0 };
    depctl.b.usbactep = true;
    depctl.b.nextep = regs->inep_regs[0].diepctl.b.nextep;
    regs->outep_regs[0].doepctl.d32 = depctl.d32;
    regs->inep_regs[0].diepctl.d32 = depctl.d32;
}

void DWOTG::handleIrq()
{
    DWOTGRegs::gintsts gintsts = { regs->gregs.gintsts.d32 };

    if (gintsts.b.usbreset)
    {
        regs->dregs.dcfg.b.devaddr = 0;
        handleBusReset(false);
    }

    if (gintsts.b.enumdone)
    {
        handleBusReset(regs->dregs.dsts.b.enumspd == 0);
        ep0Init();
    }

    if (gintsts.b.rxstsqlvl && !useDma)
    {
        // Device to memory part of the "software DMA" implementation, used to receive data if use_dma == 0.
        // Handle one packet at a time, the IRQ will re-trigger if there's something left.
        DWOTGRegs::grxfsts rxsts = { regs->gregs.grxstsp.d32 };
        int ep = rxsts.b.chnum;
        int words = (rxsts.b.bcnt + 3) >> 2;
        while (words--) *endpoints[ep].rxAddr++ = regs->dfifo[0][0];
    }

    if (gintsts.b.nptxfempty && regs->gregs.gintmsk.b.nptxfempty)
    {
        // Old style, "shared TX FIFO" memory to device part of the "software DMA" implementation,
        // used to send data if use_dma == 0 and the device doesn't support one non-periodic TX FIFO per endpoint.

        // First disable the IRQ, it will be re-enabled later if there is anything left to be done.
        regs->gregs.gintmsk.b.nptxfempty = false;

        // Check all endpoints for anything to be transmitted
        int ep;
        for (ep = 0; ep < 16; ep++) tryPush(ep);
    }

    if (gintsts.b.inepintr)
    {
        DWOTGRegs::daint daint = { regs->dregs.daint.d32 };
        int ep;
        for (ep = 0; ep < 16; ep++)
            if (daint.ep.in & (1 << ep))
            {
                DWOTGRegs::diepintn epints = { regs->inep_regs[ep].diepint.d32 };
                if (epints.b.emptyintr)
                {
                    // Memory to device part of the "software DMA" implementation, used to transmit data if use_dma == 0.
                    DWOTGRegs::depxfrsiz deptsiz = { regs->inep_regs[ep].dieptsiz.d32 };
                    if (!deptsiz.b.xfersize) regs->dregs.diepempmsk.ep.in &= ~(1 << ep);
                    else
                    {
                        // Push data into the TX FIFO until we don't have anything left or the FIFO would overflow.
                        int left = (deptsiz.b.xfersize + 3) >> 2;
                        while (left)
                        {
                            int words = regs->inep_regs[ep].dtxfsts.b.txfspcavail;
                            if (words > left) words = left;
                            if (!words) break;
                            left -= words;
                            while (words--) regs->dfifo[ep][0] = *endpoints[ep].txAddr++;
                        }
                    }
                }
                int bytesleft = regs->inep_regs[ep].dieptsiz.b.xfersize;
                regs->inep_regs[ep].diepint.d32 = epints.d32;
                if (epints.b.xfercompl) handleXferComplete(::USB::EndpointNumber(::USB::In, ep), bytesleft);
            }
    }

    if (gintsts.b.outepintr)
    {
        DWOTGRegs::daint daint = { regs->dregs.daint.d32 };
        int ep;
        for (ep = 0; ep < 16; ep++)
            if (daint.ep.out & (1 << ep))
            {
                DWOTGRegs::doepintn epints = { regs->outep_regs[ep].doepint.d32 };
                regs->outep_regs[ep].doepint.d32 = epints.d32;
                if (epints.b.setup)
                {
                    if (useDma) invalidate_dcache((const void*)regs->inep_regs[ep].diepdma, 8);
                    flushInEndpoint(ep);
                    uint32_t offset = 8 * (2 - regs->outep_regs[ep].doeptsiz.b.mc);
                    handleSetupReceived(::USB::EndpointNumber(::USB::Out, ep), offset);
                }
                else if (epints.b.xfercompl)
                {
                    int bytesleft = regs->inep_regs[ep].dieptsiz.b.xfersize;
                    handleXferComplete(::USB::EndpointNumber(::USB::Out, ep), bytesleft);
                }
            }
    }

    regs->gregs.gintsts.d32 = gintsts.d32;
}

void DWOTG::drvStart()
{
    int i;

    // Disable IRQ during setup
    socDisableIrq();

    // Enable OTG clocks
    socEnableClocks();

    // Enable PHY clocks
    DWOTGRegs::pcgcctl pcgcctl = { 0 };
    regs->pcgcctl.d32 = pcgcctl.d32;

    // Configure PHY type (must be done before reset)
    DWOTGRegs::gccfg gccfg = { 0 };
    gccfg.b.disablevbussensing = true;
    gccfg.b.pwdn = false;
    regs->gregs.gccfg.d32 = gccfg.d32;
    DWOTGRegs::gusbcfg gusbcfg = { 0 };
    gusbcfg.b.force_dev = true;
    gusbcfg.b.usbtrdtim = DWOTG_TURNAROUND;
    if (phy16bit) gusbcfg.b.phyif = true;
    else if (phyUlpi) gusbcfg.b.ulpi_utmi_sel = true;
    else gusbcfg.b.physel  = true;
    regs->gregs.gusbcfg.d32 = gusbcfg.d32;

    // Reset the whole USB core
    DWOTGRegs::grstctl grstctl = { 0 };
    grstctl.b.csftrst = true;
    udelay(100);
    while (!regs->gregs.grstctl.b.ahbidle);
    regs->gregs.grstctl.d32 = grstctl.d32;
    while (regs->gregs.grstctl.b.csftrst);
    while (!regs->gregs.grstctl.b.ahbidle);

    // Soft disconnect
    DWOTGRegs::dctl dctl = { 0 };
    dctl.b.sftdiscon = 1;
    regs->dregs.dctl.d32 = dctl.d32;

    // Configure the core
    DWOTGRegs::gahbcfg gahbcfg = { 0 };
    gahbcfg.b.dmaenable = useDma;
    gahbcfg.b.hburstlen = DWOTG_AHB_BURST_LEN;
    gahbcfg.b.glblintrmsk = true;
    if (disableDoubleBuffering)
    {
        gahbcfg.b.nptxfemplvl_txfemplvl = 1;
        gahbcfg.b.ptxfemplvl = 1;
    }
    regs->gregs.gahbcfg.d32 = gahbcfg.d32;
    regs->gregs.gusbcfg.d32 = gusbcfg.d32;
    gccfg.b.pwdn = 1;
    regs->gregs.gccfg.d32 = gccfg.d32;
    DWOTGRegs::dcfg dcfg = { 0 };
    dcfg.b.nzstsouthshk = true;
    regs->dregs.dcfg.d32 = dcfg.d32;

    // Configure the FIFOs
    if (useDma)
    {
        DWOTGRegs::dthrctl dthrctl = { 0 };
        dthrctl.b.arb_park_en = true;
        dthrctl.b.rx_thr_en = true;
        dthrctl.b.iso_thr_en = false;
        dthrctl.b.non_iso_thr_en = false;
        dthrctl.b.rx_thr_len = DWOTG_AHB_THRESHOLD;
        regs->dregs.dthrctl.d32 = dthrctl.d32;
    }
    int addr = totalFifoSize;
    for (i = 0; i < fifoCount; i++)
    {
        regs->inep_regs[i].diepctl.b.nextep = (i + 1) & 0xf;
        int size = fifoSizeList[i];
        addr -= size;
        if (size)
        {
            DWOTGRegs::txfsiz fsiz = { 0 };
            fsiz.b.startaddr = addr;
            fsiz.b.depth = size;
            if (!i) regs->gregs.dieptxf0_hnptxfsiz.d32 = fsiz.d32;
            else regs->gregs.dieptxf[i - 1].d32 = fsiz.d32;
        }
    }
    DWOTGRegs::rxfsiz fsiz = { 0 };
    fsiz.b.depth = addr;
    regs->gregs.grxfsiz.d32 = fsiz.d32;

    // Set up interrupts
    DWOTGRegs::doepintn doepmsk =  { 0 };
    doepmsk.b.xfercompl = true;
    doepmsk.b.setup = true;
    regs->dregs.doepmsk.d32 = doepmsk.d32;
    DWOTGRegs::diepintn diepmsk =  { 0 };
    diepmsk.b.xfercompl = true;
    regs->dregs.diepmsk.d32 = diepmsk.d32;
    regs->dregs.diepempmsk.d32 = 0;
    DWOTGRegs::daint daintmsk = { 0 };
    daintmsk.ep.in = 0b0000000000000001;
    daintmsk.ep.out = 0b0000000000000001;
    regs->dregs.daintmsk.d32 = daintmsk.d32;
    DWOTGRegs::gintmsk gintmsk =  { 0 };
    gintmsk.b.usbreset = true;
    gintmsk.b.enumdone = true;
    gintmsk.b.outepintr = true;
    gintmsk.b.inepintr = true;
    if (!useDma) gintmsk.b.rxstsqlvl = true;
    regs->gregs.gintmsk.d32 = gintmsk.d32;
    flushIrqs();
    socClearIrq();
    socEnableIrq();

    // Soft reconnect
    dctl.b.sftdiscon = 0;
    regs->dregs.dctl.d32 = dctl.d32;
}

void DWOTG::drvStop()
{
    // Soft disconnect
    DWOTGRegs::dctl dctl = { 0 };
    dctl.b.sftdiscon = 1;
    regs->dregs.dctl.d32 = dctl.d32;

    // Disable IRQs
    socDisableIrq();

    // Disable clocks
    socDisableClocks();
}

int DWOTG::getMaxTransferSize(::USB::EndpointNumber ep)
{
    return 1023;
}
