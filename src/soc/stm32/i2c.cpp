#include "global.h"
#include "soc/stm32/i2c.h"
#include "soc/stm32/i2c_regs.h"
#include "cpu/arm/cortexm/irq.h"
#include "cpu/arm/cortexm/cmsis.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"

#if defined(SOC_STM32F0)
#include "soc/stm32/f0/rcc.h"
#elif defined(SOC_STM32F2) || defined(SOC_STM32F4)
#include "soc/stm32/f2/rcc.h"
#endif


namespace STM32
{
    IRQn_Type const i2c_irq[][2] =
    {
        { i2c1_ev_IRQn, i2c1_er_IRQn },
        { i2c2_ev_IRQn, i2c2_er_IRQn },
        { i2c3_ev_IRQn, i2c3_er_IRQn },
    };

    int I2C::setFrequency(int frequency)
    {
        volatile STM32_I2C_REG_TYPE* regs = &STM32_I2C_REGS(index);
        int baseclock = RCC::getAPBClockFrequency(RCC::APB1);
        if (baseclock > 46000000) return ::I2C::RESULT_UNSUPPORTED;
        int divider = MIN(4095, MAX(4, (baseclock / frequency + 2) / 3));
        frequency = baseclock / divider;
        clockgate_enable(STM32_I2C_CLOCKGATE(index), true);
        union STM32_I2C_REG_TYPE::CR1 CR1 = { 0 };
        if (!initialized)
        {
            initialized = true;
            CR1.b.SWRST = true;
            regs->CR1.d32 = CR1.d32;
            CR1.d32 = 0;
        }
        regs->CR1.d32 = CR1.d32;
        union STM32_I2C_REG_TYPE::CR2 CR2 = { 0 };
        CR2.b.FREQ = baseclock / 1000000;
        CR2.b.ITBUFEN = true;
        CR2.b.ITEVTEN = true;
        CR2.b.ITERREN = true;
        regs->CR2.d32 = CR2.d32;
        regs->TRISE = divider >> 3;
        union STM32_I2C_REG_TYPE::CCR CCR = { 0 };
        CCR.b.CCR = divider;
        if (frequency > 100000) CCR.b.FS = true;
        regs->CCR.d32 = CCR.d32;
        CR1.b.PE = true;
        regs->CR1.d32 = CR1.d32;
        clockgate_enable(STM32_I2C_CLOCKGATE(index), false);
        NVIC_ClearPendingIRQ(i2c_irq[index][0]);
        NVIC_ClearPendingIRQ(i2c_irq[index][1]);
        NVIC_EnableIRQ(i2c_irq[index][0]);
        NVIC_EnableIRQ(i2c_irq[index][1]);
        return ::I2C::RESULT_OK;
    }

    enum ::I2C::Result I2C::txn(const ::I2C::Transaction* txn)
    {
        volatile STM32_I2C_REG_TYPE* regs = &STM32_I2C_REGS(index);
        if (!initialized) setFrequency(400000);
        error = false;
        curTxn = txn;
        first = true;
        busy = true;
        clockgate_enable(STM32_I2C_CLOCKGATE(index), true);
        union STM32_I2C_REG_TYPE::CR1 CR1 = { regs->CR1.d32 };
        CR1.b.STOP = false;
        CR1.b.START = true;
        regs->CR1.d32 = CR1.d32;
        while (busy) idle();
        clockgate_enable(STM32_I2C_CLOCKGATE(index), false);
        return error ?  ::I2C::RESULT_NAK :  ::I2C::RESULT_OK;
    }

    void I2C::advance()
    {
        while (xfer < curTxn->transferCount)
        {
            if (curTxn->transfers[xfer].type == ::I2C::Transaction::Transfer::TYPE_TX) tx = true;
            else if (curTxn->transfers[xfer].type == ::I2C::Transaction::Transfer::TYPE_RX) tx = false;
            else if (!curTxn->transfers[xfer].len)
            {
                xfer++;
                continue;
            }
            buf = (uint8_t*)curTxn->transfers[xfer].rxbuf;
            len = curTxn->transfers[xfer].len;
            break;
        }
        if (xfer >= curTxn->transferCount) len = 0;
        totallen = len;
        int i;
        for (i = xfer + 1; i < curTxn->transferCount; i++)
            if (curTxn->transfers[i].type == ::I2C::Transaction::Transfer::TYPE_CONT)
                totallen += curTxn->transfers[i].len;
            else break;
        last = i == curTxn->transferCount;
    }

    void I2C::advanceIfNecessary()
    {
        while (!len && totallen)
        {
            xfer++;
            advance();
        }
    }

    void I2C::ev_handler()
    {
        volatile STM32_I2C_REG_TYPE* regs = &STM32_I2C_REGS(index);
        union STM32_I2C_REG_TYPE::SR1 SR1 = { regs->SR1.d32 };
        if (SR1.b.SB)
        {
            uint8_t byte;
            if (curTxn->address > 0x77) byte = ((0x78 | (curTxn->address >> 8))) << 1;
            else byte = (curTxn->address) << 1;
            if (first)
            {
                tx = true;
                xfer = 0;
                advance();
            }
            if (tx) first = false;
            else if (curTxn->address < 0x78 || !first)
            {
                first = false;
                byte |= 1;
            }
            regs->DR = byte;
        }
        else if (SR1.b.ADD10) regs->DR = curTxn->address;
        else if (SR1.b.ADDR)
        {
            if (!curTxn->transferCount)
            {
                discard(regs->SR2.d32);
                union STM32_I2C_REG_TYPE::CR1 CR1 = { regs->CR1.d32 };
                CR1.b.STOP = true;
                regs->CR1.d32 = CR1.d32;
                busy = false;
            }
            else if (first)
            {
                first = false;
                discard(regs->SR2.d32);
                union STM32_I2C_REG_TYPE::CR1 CR1 = { regs->CR1.d32 };
                CR1.b.START = true;
                regs->CR1.d32 = CR1.d32;
            }
            else if (!tx && totallen > 1)
            {
                union STM32_I2C_REG_TYPE::CR1 CR1 = { regs->CR1.d32 };
                CR1.b.ACK = true;
                regs->CR1.d32 = CR1.d32;
                discard(regs->SR2.d32);
            }
            else if (tx && totallen)
            {
                discard(regs->SR2.d32);
                advanceIfNecessary();
                regs->DR = *buf++;
                len--;
                totallen--;
            }
            else if (!last)
            {
                discard(regs->SR2.d32);
                union STM32_I2C_REG_TYPE::CR1 CR1 = { regs->CR1.d32 };
                CR1.b.START = true;
                regs->CR1.d32 = CR1.d32;
                if (!totallen)
                {
                    xfer++;
                    advance();
                }
                while (regs->CR1.b.START);
            }
            else
            {
                discard(regs->SR2.d32);
                union STM32_I2C_REG_TYPE::CR1 CR1 = { regs->CR1.d32 };
                CR1.b.STOP = true;
                regs->CR1.d32 = CR1.d32;
                if (!totallen) busy = false;
            }
        }
        else if (SR1.b.RXNE)
        {
            if (totallen == 2)
            {
                union STM32_I2C_REG_TYPE::CR1 CR1 = { regs->CR1.d32 };
                CR1.b.ACK = false;
                if (last) CR1.b.STOP = true;
                else CR1.b.START = true;
                regs->CR1.d32 = CR1.d32;
            }
            advanceIfNecessary();
            *buf++ = regs->DR;
            len--;
            totallen--;
            if (!totallen)
            {
                if (last) busy = false;
                else
                {
                    xfer++;
                    advance();
                }
            }
            while (regs->CR1.b.START);
        }
        else if (SR1.b.TXE)
        {
            if (totallen)
            {
                advanceIfNecessary();
                regs->DR = *buf++;
                len--;
                totallen--;
            }
            else if (!last)
            {
                union STM32_I2C_REG_TYPE::CR1 CR1 = { regs->CR1.d32 };
                CR1.b.START = true;
                regs->CR1.d32 = CR1.d32;
                xfer++;
                advance();
                while (regs->CR1.b.START);
            }
            else
            {
                union STM32_I2C_REG_TYPE::CR1 CR1 = { regs->CR1.d32 };
                CR1.b.STOP = true;
                regs->CR1.d32 = CR1.d32;
                busy = false;
            }
        }
    }

    void I2C::er_handler()
    {
        volatile STM32_I2C_REG_TYPE* regs = &STM32_I2C_REGS(index);
        error = true;
        discard(regs->SR1.d32);
        discard(regs->SR2.d32);
        regs->SR1.d32 = 0;
        union STM32_I2C_REG_TYPE::CR1 CR1 = { regs->CR1.d32 };
        CR1.b.STOP = true;
        regs->CR1.d32 = CR1.d32;
        busy = false;
    }

    I2C I2C::I2C1(0);
    I2C I2C::I2C2(1);
    I2C I2C::I2C3(2);

}

extern "C" void i2c1_ev_irqhandler()
{
    STM32::I2C::I2C1.ev_handler();
}

extern "C" void i2c1_er_irqhandler()
{
    STM32::I2C::I2C1.er_handler();
}

extern "C" void i2c2_ev_irqhandler()
{
    STM32::I2C::I2C2.ev_handler();
}

extern "C" void i2c2_er_irqhandler()
{
    STM32::I2C::I2C2.er_handler();
}

extern "C" void i2c3_ev_irqhandler()
{
    STM32::I2C::I2C3.ev_handler();
}

extern "C" void i2c3_er_irqhandler()
{
    STM32::I2C::I2C3.er_handler();
}
