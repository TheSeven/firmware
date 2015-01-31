#include "global.h"
#include "soc/stm32/uart.h"
#include "cpu/arm/cortexm/irq.h"
#include "interface/irq/irq.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"

#if defined(SOC_STM32F0)
#include "soc/stm32/f0/uart_regs.h"
#include "soc/stm32/f0/rcc.h"
#elif defined(SOC_STM32F1)
#include "soc/stm32/uart_regs.h"
#include "soc/stm32/f1/rcc.h"
#elif defined(SOC_STM32F2) || defined(SOC_STM32F4)
#include "soc/stm32/uart_regs.h"
#include "soc/stm32/f2/rcc.h"
#endif


namespace STM32
{

    volatile STM32_USART_REG_TYPE* const uart_regs[] =
    {
        &STM32_USART1_REGS,
        &STM32_USART2_REGS,
#if defined(SOC_STM32F1) || defined(SOC_STM32F2) || defined(SOC_STM32F4)
        &STM32_USART3_REGS,
        &STM32_UART4_REGS,
        &STM32_UART5_REGS,
#endif
#if defined(SOC_STM32F2) || defined(SOC_STM32F4)
        &STM32_USART6_REGS,
#endif
#ifdef SOC_STM32F42X
        &STM32_UART7_REGS,
        &STM32_UART8_REGS,
#endif
    };
    
    uint8_t const uart_clocks[] =
    {
        STM32_USART1_CLOCKGATE,
        STM32_USART2_CLOCKGATE,
#if defined(SOC_STM32F1) || defined(SOC_STM32F2) || defined(SOC_STM32F4)
        STM32_USART3_CLOCKGATE,
        STM32_UART4_CLOCKGATE,
        STM32_UART5_CLOCKGATE,
#endif
#if defined(SOC_STM32F2) || defined(SOC_STM32F4)
        STM32_USART6_CLOCKGATE,
#endif
#ifdef SOC_STM32F42X
        STM32_UART7_CLOCKGATE,
        STM32_UART8_CLOCKGATE,
#endif
    };
    
    uint8_t const uart_irqs[] =
    {
        usart1_IRQn,
        usart2_IRQn,
#if defined(SOC_STM32F1) || defined(SOC_STM32F2) || defined(SOC_STM32F4)
        usart3_IRQn,
        uart4_IRQn,
        uart5_IRQn,
#endif
#if defined(SOC_STM32F2) || defined(SOC_STM32F4)
        usart6_IRQn,
#endif
#ifdef SOC_STM32F42X
        uart7_IRQn,
        uart8_IRQn,
#endif
    };

#if defined(SOC_STM32F1) || defined(SOC_STM32F2) || defined(SOC_STM32F4)
    RCC::APBBus uart_buses[] =
    {
        RCC::APB2,
        RCC::APB1,
        RCC::APB1,
        RCC::APB1,
        RCC::APB1,
#if defined(SOC_STM32F2) || defined(SOC_STM32F4)
        RCC::APB2,
#endif
#ifdef SOC_STM32F42X
        RCC::APB1,
        RCC::APB1,
#endif
    };
#endif

    enum ::UART::Result UART::configure(int baudrate, int tolerance, Config config)
    {
        volatile STM32_USART_REG_TYPE* regs = uart_regs[index];
#if defined(SOC_STM32F1) || defined(SOC_STM32F2) || defined(SOC_STM32F4)
        int baseclock = RCC::getAPBClockFrequency(uart_buses[index]);
#else
        int baseclock = RCC::getAPBClockFrequency();
#endif
        int divisor = (baseclock * 2 / baudrate + 1) / 2;
        if (divisor < 8) divisor = 8;
        int actualrate = baseclock / divisor;
        if (actualrate > baudrate + tolerance || actualrate < baudrate - tolerance) return ::UART::RESULT_UNSUPPORTED;
        union STM32_USART_REG_TYPE::CR1 CR1 = { 0 };
        union STM32_USART_REG_TYPE::CR2 CR2 = { 0 };
        union STM32_USART_REG_TYPE::CR3 CR3 = { 0 };
        CR1.b.UE = true;
        CR1.b.TE = true;
        CR1.b.RE = true;
        CR1.b.RXNEIE = true;
        if (divisor < 16)
        {
            CR1.b.OVER8 = true;
            divisor = ((divisor & ~7) << 1) | (divisor & 7);
        }
        CR1.b.M = config.parity != config.ParityNone;
        CR1.b.PCE = config.parity != config.ParityNone;
        CR1.b.PS = config.parity == config.ParityOdd;
        CR2.b.STOP = (typeof(CR2.b.STOP))config.stopBits;
        CR3.b.CTSE = config.honorCTS;
        CR3.b.RTSE = config.signalRTS;
        CR3.b.HDSEL = config.halfDuplex;
        clockgate_enable(uart_clocks[index], true);
        while (!regs->SR.b.TC);
        regs->CR1.d32 = 0;
        regs->BRR.d32 = divisor;
        regs->CR3.d32 = CR3.d32;
        regs->CR2.d32 = CR2.d32;
        regs->CR1.d32 = CR1.d32;
        irq_enable(uart_irqs[index], true);
        return ::UART::RESULT_OK;
    }

    void UART::bwTx(uint8_t byte)
    {
        volatile STM32_USART_REG_TYPE* regs = uart_regs[index];
#if defined(SOC_STM32F1) || defined(SOC_STM32F2) || defined(SOC_STM32F4)
        while (!regs->SR.b.TXE);
#else
        while (!regs->ISR.b.TXE);
#endif
        union STM32_USART_REG_TYPE::CR1 CR1 = { regs->CR1.d32 };
        CR1.b.TXEIE = true;
        regs->CR1.d32 = CR1.d32;;
        regs->DR = byte;
    }

    void UART::flush()
    {
        volatile STM32_USART_REG_TYPE* regs = uart_regs[index];
#if defined(SOC_STM32F1) || defined(SOC_STM32F2) || defined(SOC_STM32F4)
        while (!regs->SR.b.TC);
#else
        while (!regs->ISR.b.TC);
#endif
    }

    void UART::irq()
    {
        volatile STM32_USART_REG_TYPE* regs = uart_regs[index];
#if defined(SOC_STM32F1) || defined(SOC_STM32F2) || defined(SOC_STM32F4)
        union STM32_USART_REG_TYPE::SR SR = { regs->SR.d32 };
#else
        union STM32_USART_REG_TYPE::ISR SR = { regs->ISR.d32 };
#endif
        if (SR.b.RXNE)
        {
            if (SR.b.ORE) bwRxCallback(0, ::UART::RESULT_OVERFLOW_ERROR);
            uint8_t data = regs->DR;
            ::UART::Result result = ::UART::RESULT_OK;
            if (SR.b.FE) result = ::UART::RESULT_FRAMING_ERROR;
            else if (SR.b.PE) result = ::UART::RESULT_PARITY_ERROR;
#if defined(SOC_STM32F1) || defined(SOC_STM32F2) || defined(SOC_STM32F4)
            else if (SR.b.NE) result = ::UART::RESULT_NOISE_ERROR;
#else
            else if (SR.b.NF) result = ::UART::RESULT_NOISE_ERROR;
#endif
            bwRxCallback(data, result);
        }
        if (SR.b.TXE)
        {
            union STM32_USART_REG_TYPE::CR1 CR1 = { regs->CR1.d32 };
            CR1.b.TXEIE = false;
            regs->CR1.d32 = CR1.d32;;
            bwTxCallback();
        }
    }

    UART UART::UART1(0);
    UART UART::UART2(1);
#if defined(SOC_STM32F1) || defined(SOC_STM32F2) || defined(SOC_STM32F4)
    UART UART::UART3(2);
    UART UART::UART4(3);
    UART UART::UART5(4);
#endif
#if defined(SOC_STM32F2) || defined(SOC_STM32F4)
    UART UART::UART6(5);
#endif
#ifdef SOC_STM32F42X
    UART UART::UART7(6);
    UART UART::UART8(7);
#endif
}

extern "C" void usart1_irqhandler()
{
    STM32::UART::UART1.irq();
}

extern "C" void usart2_irqhandler()
{
    STM32::UART::UART2.irq();
}

#if defined(SOC_STM32F1) || defined(SOC_STM32F2) || defined(SOC_STM32F4)
extern "C" void usart3_irqhandler()
{
    STM32::UART::UART3.irq();
}

extern "C" void uart4_irqhandler()
{
    STM32::UART::UART4.irq();
}

extern "C" void uart5_irqhandler()
{
    STM32::UART::UART5.irq();
}
#endif

#if defined(SOC_STM32F2) || defined(SOC_STM32F4)
extern "C" void usart6_irqhandler()
{
    STM32::UART::UART6.irq();
}
#endif

#ifdef SOC_STM32F42X
extern "C" void uart7_irqhandler()
{
    STM32::UART::UART7.irq();
}

extern "C" void uart8_irqhandler()
{
    STM32::UART::UART8.irq();
}
#endif
