#include "global.h"
#include "soc/stm32/spi.h"
#include "soc/stm32/spi_regs.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"

#if defined(SOC_STM32F0)
#include "soc/stm32/f0/rcc.h"
#elif defined(SOC_STM32F1)
#include "soc/stm32/f1/rcc.h"
#elif defined(SOC_STM32F2) || defined(SOC_STM32F4)
#include "soc/stm32/f2/rcc.h"
#endif


namespace STM32
{

    volatile STM32_SPI_REG_TYPE* const spi_regs[] =
    {
        &STM32_SPI1_REGS,
#if defined(SOC_STM32F1) || defined(SOC_STM32F2) || defined(SOC_STM32F4)
        &STM32_SPI2_REGS,
        &STM32_SPI3_REGS,
#endif
#ifdef SOC_STM32F42X
        &STM32_SPI4_REGS,
        &STM32_SPI5_REGS,
        &STM32_SPI6_REGS,
#endif
    };
    
    uint8_t const spi_clocks[] =
    {
        STM32_SPI1_CLOCKGATE,
#if defined(SOC_STM32F1) || defined(SOC_STM32F2) || defined(SOC_STM32F4)
        STM32_SPI2_CLOCKGATE,
        STM32_SPI3_CLOCKGATE,
#endif
#ifdef SOC_STM32F42X
        STM32_SPI4_CLOCKGATE,
        STM32_SPI5_CLOCKGATE,
        STM32_SPI6_CLOCKGATE,
#endif
    };
    
    int SPI::setFrequency(int frequency) const
    {
        volatile STM32_SPI_REG_TYPE* regs = spi_regs[index];
#if defined(SOC_STM32F1) || defined(SOC_STM32F2) || defined(SOC_STM32F4)
        int baseclock = RCC::getAPBClockFrequency(((spi_clocks[index] >> 5) & 1) ? RCC::APB2 : RCC::APB1);
#else
        int baseclock = RCC::getAPBClockFrequency();
#endif
        int br = 0;
        for (baseclock >>= 1; baseclock > frequency && br < 7; baseclock >>= 1) br++;
        union STM32_SPI_REG_TYPE::CR1 CR1 = { 0 };
        regs->CR1.d32 = CR1.d32;
        regs->I2SCFGR.d32 = 0;
        union STM32_SPI_REG_TYPE::CR2 CR2 = { 0 };
        CR2.b.FRXTH = true;
        regs->CR2.d32 = CR2.d32;
        CR1.b.SSM = true;
        CR1.b.SSI = true;
        CR1.b.MSTR = true;
        CR1.b.SPE = true;
        CR1.b.BR = br;
        regs->CR1.d32 = CR1.d32;
        return baseclock;
    }
    
    void SPI::select() const
    {
        clockgate_enable(spi_clocks[index], true);
    }

    void SPI::deselect() const
    {
        clockgate_enable(spi_clocks[index], false);
    }

    uint8_t SPI::pushByte(uint8_t byte) const
    {
        volatile STM32_SPI_REG_TYPE* regs = spi_regs[index];
        while (!(regs->SR.b.TXE));
        regs->DR = byte;
        while (!(regs->SR.b.RXNE));
        return regs->DR;
    }
    
    void SPI::pushBuffer(const void* inbuf, void* outbuf, int len) const
    {
        if (!len) return;
        volatile STM32_SPI_REG_TYPE* regs = spi_regs[index];
        const uint8_t* indata = (const uint8_t*)inbuf;
        uint8_t* outdata = (uint8_t*)outbuf;
        while (len--)
        {
            while (!(regs->SR.b.TXE));
            regs->DR = indata ? *indata++ : 0xff;
            while (!(regs->SR.b.RXNE));
            uint8_t result = regs->DR;
            if (outdata) *outdata++ = result;
        }
    }

    const SPI SPI::SPI1(0);
    const SPI SPI::SPI2(1);
    const SPI SPI::SPI3(2);
    const SPI SPI::SPI4(3);
    const SPI SPI::SPI5(4);
    const SPI SPI::SPI6(5);

}
