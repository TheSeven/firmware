#include "global.h"
#include "target/artnetnode/flexperiment/dmx.h"
#include "target/artnetnode/main.h"
#include "soc/stm32/uart_regs.h"
#include "cpu/arm/cortexm/irq.h"
#include "interface/irq/irq.h"
#include "interface/clockgate/clockgate.h"
#include "sys/time.h"
#include "sys/util.h"


#if defined(SOC_STM32F1)
#include "soc/stm32/f1/rcc.h"
#elif defined(SOC_STM32F2) || defined(SOC_STM32F4)
#include "soc/stm32/f2/rcc.h"
#endif


DMXDMAChannel dmxChannelInstance[DMX_IN_CHANNELS] =
{
#if defined(SOC_STM32F1)
    DMXDMAChannel(0, 0, PIN_A9, PIN_B2, false, DMAStream(0, 4, 0), DMAStream(0, 3, 0)),
    DMXDMAChannel(1, 1, PIN_C6, PIN_C13, false, DMAStream(0, 5, 0), DMAStream(0, 6, 0)),
    DMXDMAChannel(2, 2, PIN_B10, PIN_B1, true, DMAStream(0, 2, 0), DMAStream(0, 1, 0)),
    DMXDMAChannel(3, 3, PIN_C10, PIN_B0, true, DMAStream(1, 2, 0), DMAStream(1, 4, 0)),
//TODO    DMXPIOChannel(4, 4, PIN_C12, PIN_C11, true),
#elif defined(SOC_STM32F2) || defined(SOC_STM32F4)
    DMXDMAChannel(0, 0, PIN_A9, PIN_B2, false, DMAStream(1, 2, 4), DMAStream(1, 7, 4)),
    DMXDMAChannel(1, 1, PIN_C6, PIN_C13, false, DMAStream(0, 5, 4), DMAStream(0, 6, 4)),
    DMXDMAChannel(2, 2, PIN_C10, PIN_B1, true, DMAStream(0, 1, 4), DMAStream(0, 3, 4)),
    DMXDMAChannel(3, 3, PIN_A0, PIN_B0, true, DMAStream(0, 2, 4), DMAStream(0, 4, 4)),
    DMXDMAChannel(4, 4, PIN_C12, PIN_C11, true, DMAStream(0, 0, 4), DMAStream(0, 7, 4)),
    DMXDMAChannel(5, 5, PIN_C6, PIN_C7, true, DMAStream(1, 1, 5), DMAStream(1, 6, 5)),
#endif
};


DMXBidiChannel* dmxChannel[DMX_IN_CHANNELS] =
{
    dmxChannelInstance + 0,
    dmxChannelInstance + 1,
    dmxChannelInstance + 2,
    dmxChannelInstance + 3,
#if defined(SOC_STM32F2) || defined(SOC_STM32F4)
    dmxChannelInstance + 4,
    dmxChannelInstance + 5,
#endif
};

DMXInChannel* dmxInChannel[DMX_IN_CHANNELS] =
{
    dmxChannelInstance + 0,
    dmxChannelInstance + 1,
    dmxChannelInstance + 2,
    dmxChannelInstance + 3,
#if defined(SOC_STM32F2) || defined(SOC_STM32F4)
    dmxChannelInstance + 4,
    dmxChannelInstance + 5,
#endif
};

DMXOutChannel* dmxOutChannel[DMX_OUT_CHANNELS] =
{
    dmxChannelInstance + 0,
    dmxChannelInstance + 2,
    dmxChannelInstance + 3,
#if defined(SOC_STM32F2) || defined(SOC_STM32F4)
    dmxChannelInstance + 4,
    dmxChannelInstance + 5,
#endif
};


volatile STM32_USART_REG_TYPE* const uart_regs[] =
{
    &STM32_USART1_REGS,
    &STM32_USART2_REGS,
    &STM32_USART3_REGS,
    &STM32_UART4_REGS,
    &STM32_UART5_REGS,
#if defined(SOC_STM32F2) || defined(SOC_STM32F4)
    &STM32_USART6_REGS,
#endif
};

uint8_t const uart_clocks[] =
{
    STM32_USART1_CLOCKGATE,
    STM32_USART2_CLOCKGATE,
    STM32_USART3_CLOCKGATE,
    STM32_UART4_CLOCKGATE,
    STM32_UART5_CLOCKGATE,
#if defined(SOC_STM32F2) || defined(SOC_STM32F4)
    STM32_USART6_CLOCKGATE,
#endif
};

uint8_t const uart_irqs[] =
{
    usart1_IRQn,
    usart2_IRQn,
    usart3_IRQn,
    uart4_IRQn,
    uart5_IRQn,
#if defined(SOC_STM32F2) || defined(SOC_STM32F4)
    usart6_IRQn,
#endif
};

STM32::RCC::APBBus uart_buses[] =
{
    STM32::RCC::APB2,
    STM32::RCC::APB1,
    STM32::RCC::APB1,
    STM32::RCC::APB1,
    STM32::RCC::APB1,
#if defined(SOC_STM32F2) || defined(SOC_STM32F4)
    STM32::RCC::APB2,
#endif
};

uint8_t const dma_irqs[] =
{
    dma1_stream1_IRQn,
    dma1_stream2_IRQn,
    dma1_stream3_IRQn,
    dma1_stream4_IRQn,
    dma1_stream5_IRQn,
    dma1_stream6_IRQn,
    dma1_stream7_IRQn,
    dma1_stream8_IRQn,
    dma2_stream1_IRQn,
    dma2_stream2_IRQn,
    dma2_stream3_IRQn,
    dma2_stream4_IRQn,
    dma2_stream5_IRQn,
#if defined(SOC_STM32F2) || defined(SOC_STM32F4)
    dma2_stream6_IRQn,
    dma2_stream7_IRQn,
    dma2_stream8_IRQn,
#endif
};

uint8_t const dma_clocks[] =
{
    STM32_DMA1_CLOCKGATE,
    STM32_DMA2_CLOCKGATE,
};


DMXDMAChannel::DMXDMAChannel(int index, int uart, GPIO::Pin txPin, GPIO::Pin oePin, bool halfDuplex,
                             DMAStream dmaIn, DMAStream dmaOut)
    : txPin(txPin), oePin(oePin), dmaIn(dmaIn), dmaOut(dmaOut), mode(ChannelModeDisabled),
      activeInBuf(0), activeOutBuf(0), index(index), outPending(false), inBusy(false), outBusy(false)

{
    GPIO::configure(oePin, GPIO::MODE_OUTPUT, false);
    mode = ChannelModeDisabled;
    memset(inData, 0, sizeof(inData));
    memset(outData, 0, sizeof(outData));
    uartRegs = uart_regs[uart];
    dmaRegsIn = &STM32_DMA_STREAM_REGS(dmaIn.controller, dmaIn.stream);
    dmaRegsOut = &STM32_DMA_STREAM_REGS(dmaOut.controller, dmaOut.stream);
    dmaInIRQ = dma_irqs[(dmaIn.controller << 3) | dmaIn.stream];
    dmaOutIRQ = dma_irqs[(dmaOut.controller << 3) | dmaOut.stream];
    int baseclock = STM32::RCC::getAPBClockFrequency(uart_buses[uart]);
    int divisor = (baseclock * 2 / 250000 + 1) / 2;
    if (divisor < 8) divisor = 8;
    union STM32_USART_REG_TYPE::CR1 CR1 = { 0 };
    union STM32_USART_REG_TYPE::CR2 CR2 = { 0 };
    union STM32_USART_REG_TYPE::CR3 CR3 = { 0 };
    CR1.b.UE = true;
    CR1.b.TE = true;
    CR1.b.RE = true;
    CR2.b.STOP = 2;
    CR2.b.LBDIE = true;
    CR3.b.EIE = true;
    CR3.b.DMAT = true;
    CR3.b.DMAR = true;
    CR3.b.HDSEL = halfDuplex;
    clockgate_enable(uart_clocks[uart], true);
    while (!uartRegs->SR.b.TC);
    uartRegs->CR1.d32 = 0;
    uartRegs->BRR.d32 = divisor;
    uartRegs->CR3.d32 = CR3.d32;
    uartRegs->CR2.d32 = CR2.d32;
    uartRegs->CR1.d32 = CR1.d32;
    clockgate_enable(dma_clocks[dmaIn.controller], true);
    clockgate_enable(dma_clocks[dmaOut.controller], true);
    union STM32_DMA_STREAM_REG_TYPE::CR CR = { 0 };
#ifndef SOC_STM32F1
    CR.b.CHSEL = dmaIn.channel;
    CR.b.PL = 3;
    CR.b.MSIZE = 1;
#endif
    CR.b.MINC = true;
    CR.b.TCIE = true;
    dmaRegsIn->CR.d32 = CR.d32;
#ifndef SOC_STM32F1
    CR.b.CHSEL = dmaOut.channel;
    CR.b.PL = 0;
    CR.b.MSIZE = 2;
#endif
    CR.b.DIR = 1;
    dmaRegsOut->CR.d32 = CR.d32;
    dmaRegsIn->PAR = (uint32_t)&uartRegs->DR;
    dmaRegsOut->PAR = (uint32_t)&uartRegs->DR;
#ifndef SOC_STM32F1
    union STM32_DMA_STREAM_REG_TYPE::FCR FCR = { 0 };
    FCR.b.DMDIS = true;
    dmaRegsIn->FCR.d32 = FCR.d32;
    FCR.b.FTH = 2;
    dmaRegsOut->FCR.d32 = FCR.d32;
#endif
    irq_set_priority(uart_irqs[uart], 2);
    irq_set_priority(dmaInIRQ, 10);
    irq_set_priority(dmaOutIRQ, 12);
    lastSentTime = read_usec_timer();
    irq_enable(uart_irqs[uart], true);
    irq_enable(dmaInIRQ, true);
    irq_enable(dmaOutIRQ, true);
}


void DMXDMAChannel::setMode(enum ChannelMode mode)
{
    this->mode = mode;
    bool out = mode & ChannelModeOutput;
    if (!out) GPIO::setMode(txPin, GPIO::MODE_INPUT);
    GPIO::setLevel(oePin, out);
    if (out) GPIO::setMode(txPin, GPIO::MODE_SPECIAL);
}


const uint8_t* DMXDMAChannel::getInDataPtr()
{
    return inData[activeInBuf ^ 1] + 4;
}


bool DMXDMAChannel::isBusy()
{
    return outBusy;
}


uint8_t* DMXDMAChannel::getOutDataPtr()
{
    return outData[activeOutBuf ^ 1];
}


void DMXDMAChannel::sendPacket()
{
    enter_critical_section();
    outPending = true;
    bool oldBusy = outBusy;
    outBusy = true;
    leave_critical_section();
    lastSentTime = read_usec_timer();
    if (oldBusy) return;
    irq_set_pending(dmaOutIRQ);
}


void DMXDMAChannel::handleUARTIRQ()
{
    union STM32_USART_REG_TYPE::SR SR = { uartRegs->SR.d32 };
    discard(uartRegs->DR);
    if (SR.b.ORE || SR.b.NE)
    {
        if (dmaRegsIn->NDTR) inData[activeInBuf][3] = 0xfe;
    }
    else if (SR.b.LBD)
    {
        union STM32_DMA_STREAM_REG_TYPE::CR CR = { dmaRegsIn->CR.d32 };
        CR.b.EN = false;
        dmaRegsIn->CR.d32 = CR.d32;
        while (dmaRegsIn->CR.b.EN);
        bool good = !inData[activeInBuf][3];
        if (good) activeInBuf ^= 1;
        inData[activeInBuf][3] = 0xfd;
#ifdef SOC_STM32F1
        dmaRegsIn->MAR = (uint32_t)inData[activeInBuf] + 2;
#else
        dmaRegsIn->M0AR = (uint32_t)inData[activeInBuf] + 2;
#endif
        dmaRegsIn->NDTR = 514;
        CR.b.MINC = true;
        CR.b.EN = true;
        dmaRegsIn->CR.d32 = CR.d32;
        if (!good)
        {
#ifdef SOC_STM32F1
            STM32_DMA_REGS(dmaIn.controller).IFCR.d32 = 0xf << (dmaIn.stream * 4);
#else
            uint32_t ifcr = 0x3f;
            if (dmaIn.stream & 1) ifcr <<= 6;
            if (dmaIn.stream & 2) ifcr <<= 16;
            STM32_DMA_REGS(dmaIn.controller).IFCR.d32[dmaIn.stream >> 2] = ifcr;
#endif
            irq_clear_pending(dmaInIRQ);
        }
        else if (dmaRegsIn->CR.b.EN)
        {
            inData[0][0] = 1;
            irq_set_pending(dmaInIRQ);
        }
        SR.b.LBD = false;
        uartRegs->SR.d32 = SR.d32;
    }
}


void DMXDMAChannel::handleDMAInIRQ()
{
#ifdef SOC_STM32F1
    STM32_DMA_REGS(dmaIn.controller).IFCR.d32 = 0xf << (dmaIn.stream * 4);
#else
    uint32_t ifcr = 0x3f;
    if (dmaIn.stream & 1) ifcr <<= 6;
    if (dmaIn.stream & 2) ifcr <<= 16;
    STM32_DMA_REGS(dmaIn.controller).IFCR.d32[dmaIn.stream >> 2] = ifcr;
#endif
    enter_critical_section();
    int buf = activeInBuf ^ inData[0][0];
    inData[0][0] = 0;
    leave_critical_section();
    if (!(mode & ChannelModeInput)) return;
    if (inData[buf][3]) return;
    dmxInUpdated(index);
}


void DMXDMAChannel::handleDMAOutIRQ()
{
    union STM32_DMA_STREAM_REG_TYPE::CR CR = { dmaRegsOut->CR.d32 };
    CR.b.EN = false;
    dmaRegsOut->CR.d32 = CR.d32;
    while (dmaRegsOut->CR.b.EN);
#ifdef SOC_STM32F1
    STM32_DMA_REGS(dmaIn.controller).IFCR.d32 = 0xf << (dmaOut.stream * 4);
#else
    uint32_t ifcr = 0x3f;
    if (dmaOut.stream & 1) ifcr <<= 6;
    if (dmaOut.stream & 2) ifcr <<= 16;
    STM32_DMA_REGS(dmaIn.controller).IFCR.d32[dmaOut.stream >> 2] = ifcr;
#endif
    enter_critical_section();
    if (!outPending)
    {
        outBusy = false;
        activeOutBuf ^= 1;
        leave_critical_section();
        return;
    }
    outPending = false;
    activeOutBuf ^= 1;
    leave_critical_section();
    while (!uartRegs->SR.b.TC);
    GPIO::configure(txPin, GPIO::MODE_OUTPUT, false);
    udelay(48);
    union STM32_USART_REG_TYPE::CR1 CR1 = { uartRegs->CR1.d32 };
    CR1.b.SBK = true;
    uartRegs->CR1.d32 = CR1.d32;
    udelay(3);
    GPIO::setMode(txPin, GPIO::MODE_SPECIAL);
    uartRegs->DR = 0;
    while (!uartRegs->SR.b.TXE);
#ifdef SOC_STM32F1
    dmaRegsOut->MAR = (uint32_t)outData[activeOutBuf];
#else
    dmaRegsOut->M0AR = (uint32_t)outData[activeOutBuf];
#endif
    dmaRegsOut->NDTR = 512;
    CR.b.EN = true;
    dmaRegsOut->CR.d32 = CR.d32;
}


extern "C" void usart1_irqhandler()
{
    dmxChannelInstance[0].handleUARTIRQ();
}

extern "C" void usart2_irqhandler()
{
    dmxChannelInstance[1].handleUARTIRQ();
}

extern "C" void usart3_irqhandler()
{
    dmxChannelInstance[2].handleUARTIRQ();
}

extern "C" void uart4_irqhandler()
{
    dmxChannelInstance[3].handleUARTIRQ();
}

extern "C" void uart5_irqhandler()
{
    dmxChannelInstance[4].handleUARTIRQ();
}

extern "C" void usart6_irqhandler()
{
    dmxChannelInstance[5].handleUARTIRQ();
}

#if defined(SOC_STM32F1)
extern "C" void dma1_stream2_irqhandler()
{
    dmxChannelInstance[2].handleDMAOutIRQ();
}

extern "C" void dma1_stream3_irqhandler()
{
    dmxChannelInstance[2].handleDMAInIRQ();
}

extern "C" void dma1_stream4_irqhandler()
{
    dmxChannelInstance[0].handleDMAOutIRQ();
}

extern "C" void dma1_stream5_irqhandler()
{
    dmxChannelInstance[0].handleDMAInIRQ();
}

extern "C" void dma1_stream6_irqhandler()
{
    dmxChannelInstance[1].handleDMAInIRQ();
}

extern "C" void dma1_stream7_irqhandler()
{
    dmxChannelInstance[1].handleDMAOutIRQ();
}

extern "C" void dma2_stream3_irqhandler()
{
    dmxChannelInstance[3].handleDMAInIRQ();
}

extern "C" void dma2_stream5_irqhandler()
{
    dmxChannelInstance[3].handleDMAOutIRQ();
}
#elif defined(SOC_STM32F2) || defined(SOC_STM32F4)
extern "C" void dma1_stream1_irqhandler()
{
    dmxChannelInstance[4].handleDMAInIRQ();
}

extern "C" void dma1_stream2_irqhandler()
{
    dmxChannelInstance[2].handleDMAInIRQ();
}

extern "C" void dma1_stream3_irqhandler()
{
    dmxChannelInstance[3].handleDMAInIRQ();
}

extern "C" void dma1_stream4_irqhandler()
{
    dmxChannelInstance[2].handleDMAOutIRQ();
}

extern "C" void dma1_stream5_irqhandler()
{
    dmxChannelInstance[3].handleDMAOutIRQ();
}

extern "C" void dma1_stream6_irqhandler()
{
    dmxChannelInstance[1].handleDMAInIRQ();
}

extern "C" void dma1_stream7_irqhandler()
{
    dmxChannelInstance[1].handleDMAOutIRQ();
}

extern "C" void dma1_stream8_irqhandler()
{
    dmxChannelInstance[4].handleDMAOutIRQ();
}

extern "C" void dma2_stream2_irqhandler()
{
    dmxChannelInstance[5].handleDMAInIRQ();
}

extern "C" void dma2_stream3_irqhandler()
{
    dmxChannelInstance[0].handleDMAInIRQ();
}

extern "C" void dma2_stream7_irqhandler()
{
    dmxChannelInstance[5].handleDMAOutIRQ();
}

extern "C" void dma2_stream8_irqhandler()
{
    dmxChannelInstance[0].handleDMAOutIRQ();
}
#endif


void initDMX()
{
    dmxChannel[0]->setMode(ChannelModeBidirectional);
    dmxChannel[1]->setMode(ChannelModeInput);
    dmxChannel[2]->setMode(ChannelModeOutput);
    dmxChannel[3]->setMode(ChannelModeOutput);
#if defined(SOC_STM32F2) || defined(SOC_STM32F4)
    dmxChannel[4]->setMode(ChannelModeOutput);
    dmxChannel[5]->setMode(ChannelModeOutput);
#endif
}
