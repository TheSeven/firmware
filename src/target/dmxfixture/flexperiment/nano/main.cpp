#include "global.h"
#include "app/main.h"
#include "output.h"
#include "soc/stm32/f0/rcc.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/f0/uart_regs.h"
#include "soc/stm32/f1/dma_regs.h"
#include "cpu/arm/cortexm/irq.h"
#include "interface/irq/irq.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"


uint8_t inData[2][516];
uint8_t activeInBuf;
bool inBusy;

int main()
{
    output_init();
    memset(inData, 0, sizeof(inData));
    int baseclock = STM32::RCC::getAPBClockFrequency();
    int divisor = (baseclock * 2 / 250000 + 1) / 2;
    if (divisor < 8) divisor = 8;
    union STM32_USART_REG_TYPE::CR1 CR1 = { 0 };
    union STM32_USART_REG_TYPE::CR2 CR2 = { 0 };
    union STM32_USART_REG_TYPE::CR3 CR3 = { 0 };
    CR1.b.UE = true;
    CR1.b.TE = true;
    CR1.b.RE = true;
    CR2.b.STOP = 2;
    CR3.b.EIE = true;
    CR3.b.DMAR = true;
    clockgate_enable(STM32_USART1_CLOCKGATE, true);
    while (!STM32_USART1_REGS.ISR.b.TC);
    STM32_USART1_REGS.CR1.d32 = 0;
    STM32_USART1_REGS.BRR.d32 = divisor;
    STM32_USART1_REGS.CR3.d32 = CR3.d32;
    STM32_USART1_REGS.CR2.d32 = CR2.d32;
    STM32_USART1_REGS.CR1.d32 = CR1.d32;
    clockgate_enable(STM32_DMA_CLOCKGATE, true);
    union STM32_DMA_STREAM_REG_TYPE::CR CR = { 0 };
    CR.b.MINC = true;
    CR.b.TCIE = true;
    STM32_DMA1_STREAM2_REGS.CR.d32 = CR.d32;
    STM32_DMA1_STREAM2_REGS.PAR = (uint32_t)&STM32_USART1_REGS.RDR;
    irq_set_priority(usart1_IRQn, 2);
    irq_set_priority(dma1_stream2_3_IRQn, 10);
    irq_enable(usart1_IRQn, true);
    irq_enable(dma1_stream2_3_IRQn, true);

    while (true) idle();
}

extern "C" void usart1_irqhandler()
{
    union STM32_USART_REG_TYPE::ISR ISR = { STM32_USART1_REGS.ISR.d32 };
    discard(STM32_USART1_REGS.RDR);
    if (ISR.b.ORE || ISR.b.NF)
    {
        if (STM32_DMA1_STREAM2_REGS.NDTR) inData[activeInBuf][3] = 0xfe;
        union STM32_USART_REG_TYPE::ICR ICR = { 0 };
        ICR.b.NF = true;
        ICR.b.ORE = true;
        STM32_USART1_REGS.ICR.d32 = ICR.d32;
    }
    else if (ISR.b.FE)
    {
        union STM32_DMA_STREAM_REG_TYPE::CR CR = { STM32_DMA1_STREAM2_REGS.CR.d32 };
        CR.b.EN = false;
        STM32_DMA1_STREAM2_REGS.CR.d32 = CR.d32;
        while (STM32_DMA1_STREAM2_REGS.CR.b.EN);
        bool good = !inData[activeInBuf][3];
        if (good) activeInBuf ^= 1;
        inData[activeInBuf][3] = 0xfd;
        STM32_DMA1_STREAM2_REGS.MAR = (uint32_t)inData[activeInBuf] + 3;
        STM32_DMA1_STREAM2_REGS.NDTR = 513;
        if (!good)
        {
            STM32_DMA_REGS(0).IFCR.d32 = 0xf00;
            irq_clear_pending(dma1_stream2_3_IRQn);
        }
        else if (STM32_DMA1_STREAM2_REGS.CR.b.EN)
        {
            inData[0][0] = 1;
            irq_set_pending(dma1_stream2_3_IRQn);
        }
        while (!GPIO::getLevel(PIN_A3));
        CR.b.MINC = true;
        CR.b.EN = true;
        STM32_DMA1_STREAM2_REGS.CR.d32 = CR.d32;
        union STM32_USART_REG_TYPE::ICR ICR = { 0 };
        ICR.b.FE = true;
        STM32_USART1_REGS.ICR.d32 = ICR.d32;
    }
}

extern "C" void dma1_stream2_3_irqhandler()
{
    STM32_DMA_REGS(0).IFCR.d32 = 0xf00;
    enter_critical_section();
    int buf = activeInBuf ^ inData[0][0];
    inData[0][0] = 0;
    leave_critical_section();
    if (inData[buf][3]) return;
    output_handle_dmx(inData[buf] + 4);
}
