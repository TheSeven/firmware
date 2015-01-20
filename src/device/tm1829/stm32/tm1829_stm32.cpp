#include "global.h"
#include "device/tm1829/stm32/tm1829_stm32.h"
#include "sys/time.h"
#include "sys/util.h"
#include "interface/clockgate/clockgate.h"
#include "cpu/arm/cortexm/irq.h"
#include "soc/stm32/f0/clockgates.h"


static volatile STM32_TIM_REG_TYPE* const timer_regs[] =
{
    &STM32_TIM1_REGS,
    &STM32_TIM3_REGS,
    &STM32_TIM6_REGS,
    &STM32_TIM14_REGS,
    &STM32_TIM15_REGS,
    &STM32_TIM16_REGS,
    &STM32_TIM17_REGS,
};

uint8_t const timer_clocks[] =
{
    STM32_TIM1_CLOCKGATE,
    STM32_TIM3_CLOCKGATE,
    STM32_TIM6_CLOCKGATE,
    STM32_TIM14_CLOCKGATE,
    STM32_TIM15_CLOCKGATE,
    STM32_TIM16_CLOCKGATE,
    STM32_TIM17_CLOCKGATE,
};

static volatile STM32_DMA_REG_TYPE* const dma_regs[] =
{
    &STM32_DMA1_REGS,
};

uint8_t const dma_clocks[] =
{
    STM32_DMA_CLOCKGATE,
};

TM1829::STM32Driver::STM32Driver(const TM1829::STM32Driver::Config* config)
    : pixelsPerIRQ(config->pixelsPerIRQ), parallel(config->parallel), timeLow(config->timeLow),
      timeHigh(config->timeHigh), dmaBuffer((uint8_t*)config->dmaBuffer), dmaChannel(config->dmaChannel),
      last(0), pixelsPending(0), pixelData(NULL)
{
    timer = timer_regs[config->timer];
    dma = dma_regs[config->dmaController];
    clockgate_enable(timer_clocks[config->timer], true);
    clockgate_enable(dma_clocks[config->dmaController], true);
    dmaStream = (volatile STM32_DMA_STREAM_REG_TYPE*)(((uint32_t)dma) + 8 + 0x14 * dmaChannel);
    dmaStream->CPAR = &STM32_TIM1_REGS.DMAR;
    dmaStream->CMAR = dmaBuffer;
    union STM32_TIM_REG_TYPE::DCR DCR = { 0 };
    DCR.b.DBL = parallel - 1;
    DCR.b.DBA = 0xd + config->channel;
    timer->DCR.d32 = DCR.d32;
    union STM32_TIM_REG_TYPE::DIER DIER = { 0 };
    DIER.b.UDE = true;
    timer->DIER.d32 = DIER.d32;
    union STM32_TIM_REG_TYPE::SMCR SMCR = { 0 };
    timer->SMCR.d32 = SMCR.d32;
    union STM32_TIM_REG_TYPE::CCER CCER = { 0 };
    timer->CCER.d32 = CCER.d32;
    union STM32_TIM_REG_TYPE::CCMR1 CCMR1 = { 0 };
    CCMR1.oc.OC1PE = true;
    CCMR1.oc.OC1M = 6;
    CCMR1.oc.OC2PE = true;
    CCMR1.oc.OC2M = 6;
    timer->CCMR1.d32 = CCMR1.d32;
    union STM32_TIM_REG_TYPE::CCMR2 CCMR2 = { 0 };
    CCMR2.oc.OC3PE = true;
    CCMR2.oc.OC3M = 6;
    CCMR2.oc.OC4PE = true;
    CCMR2.oc.OC4M = 6;
    timer->CCMR2.d32 = CCMR2.d32;
    CCER.b.CC1E = true;
    CCER.b.CC2E = true;
    CCER.b.CC3E = true;
    CCER.b.CC4E = true;
    CCER.b.CC1P = true;
    CCER.b.CC2P = true;
    CCER.b.CC3P = true;
    CCER.b.CC4P = true;
    timer->CCER.d32 = CCER.d32;
    union STM32_TIM_REG_TYPE::BDTR BDTR = { 0 };
    BDTR.b.MOE = true;
    timer->BDTR.d32 = BDTR.d32;
    union STM32_TIM_REG_TYPE::CR2 CR2 = { 0 };
    timer->CR2.d32 = CR2.d32;
    union STM32_TIM_REG_TYPE::CR1 CR1 = { 0 };
    timer->CR1.d32 = CR1.d32;
    timer->PSC = config->prescaler;
    timer->ARR = config->timeSlot;
    timer->RCR = 0;
}

void TM1829::STM32Driver::sendFrame(void** data, int pixels)
{
    cancelSending();
    pixelData = (uint8_t**)data;
    pixelsPending = pixels;
    last = 0;
    fillDmaBuffer(dmaBuffer);
    dmaStream->CNDTR = 2 * parallel * 24 * pixelsPerIRQ;
    union STM32_DMA_STREAM_REG_TYPE::CCR CCR = { 0 };
    CCR.b.PL = 3;
    CCR.b.PSIZE = 1;
    CCR.b.MINC = true;
    CCR.b.CIRC = true;
    CCR.b.DIR = 1;
    CCR.b.HTIE = true;
    CCR.b.TCIE = true;
    CCR.b.EN = true;
    dmaStream->CCR.d32 = CCR.d32;
    fillDmaBuffer(dmaBuffer + 24 * pixelsPerIRQ * parallel);
    union STM32_TIM_REG_TYPE::EGR EGR = { 0 };
    EGR.b.UG = true;
    timer->EGR.d32 = EGR.d32;
    union STM32_TIM_REG_TYPE::CR1 CR1 = { 0 };
    CR1.b.CEN = true;
    timer->CR1.d32 = CR1.d32;
}

bool TM1829::STM32Driver::isSending()
{
    return timer->CR1.b.CEN;
}

void TM1829::STM32Driver::cancelSending()
{
    timer->CR1.d32 = 0;
    dmaStream->CCR.d32 = 0;
}

void TM1829::STM32Driver::handleIrq()
{
    union STM32_DMA_REG_TYPE::ISR ISR = { (dma->ISR.d32 >> (dmaChannel * 4)) & 0xf };
    if (last >= 2) cancelSending();
    else
    {
        if (ISR.b.HTIF1) fillDmaBuffer(dmaBuffer);
        if (ISR.b.TCIF1) fillDmaBuffer(dmaBuffer + 24 * pixelsPerIRQ * parallel);
    }
    dma->IFCR.d32 = ISR.d32 << (dmaChannel * 4);
}

void __attribute__((optimize("-Os","-fomit-frame-pointer"))) TM1829::STM32Driver::fillDmaBuffer(uint8_t* buffer)
{
    register int bytes asm("r12") = MIN(pixelsPending, pixelsPerIRQ) * 3;
    register int parallel asm("r9") = this->parallel;
    register uint8_t timeLow asm("r10") = this->timeLow;
    register uint8_t timeHigh asm("r11") = this->timeHigh;
    register uint8_t** dataIn asm("r7") = this->pixelData;
    register int strip asm("r0"), byte asm("r2"), bit asm("r3"), time asm("r4"), data asm("r5");
    register uint8_t* dataPtr asm("r6");
    register uint8_t* bufferBackup asm("r8");
    if (bytes)
        asm volatile(
            "mov %[bufferBackup], %[buffer]\n"
            "mov %[strip], #0\n"
            "1:\n"
            "    mov %[buffer], %[bufferBackup]\n"
            "    ldr %[dataPtr], [%[dataIn]]\n"
            "    mov %[byte], %[bytes]\n"
            "    2:\n"
            "        ldrb %[data], [%[dataPtr]]\n"
            "        add %[dataPtr], #1\n"
            "        lsl %[data], #24\n"
            "        mov %[bit], #8\n"
            "        3:\n"
            "            cmp %[data], #0\n"
            "            mov %[time], %[timeLow]\n"
            "            bpl 4f\n"
            "                mov %[time], %[timeHigh]\n"
            "            4:\n"
            "            strb %[time], [%[buffer],%[strip]]\n"
            "            lsl %[data], #1\n"
            "            add %[buffer], %[parallel]\n"
            "            sub %[bit], #1\n"
            "            bne 3b\n"
            "        sub %[byte], #1\n"
            "        bne 2b\n"
            "    str %[dataPtr], [%[dataIn]]\n"
            "    add %[dataIn], #4\n"
            "    add %[strip], #1\n"
            "    cmp %[strip], %[parallel]\n"
            "    bne 1b\n"
            : [strip]"=&l"(strip), [byte]"=&l"(byte), [bit]"=&l"(bit), [dataPtr]"=&l"(dataPtr), [dataIn]"+l"(dataIn),
              [time]"=&l"(time), [data]"=&l"(data), [buffer]"+l"(buffer), [bufferBackup]"=&h"(bufferBackup)
            : [timeLow]"h"(timeLow), [timeHigh]"h"(timeHigh), [parallel]"h"(parallel), [bytes]"h"(bytes)
            : "cc"
        );
    if (pixelsPerIRQ > pixelsPending)
    {
        memset(buffer, 0, (pixelsPerIRQ - pixelsPending) * 24 * parallel);
        pixelsPending = 0;
        last++;
    }
    else pixelsPending -= pixelsPerIRQ;
}
