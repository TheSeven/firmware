#include "global.h"
#include "soc/stm32/f0/i2c.h"
#include "soc/stm32/f0/i2c_regs.h"
#include "soc/stm32/f0/rcc.h"
#include "cpu/arm/cortexm/irq.h"
#include "cpu/arm/cortexm/cmsis.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"


namespace STM32
{
    IRQn_Type const i2c_irq[] =
    {
        i2c1_IRQn,
        i2c2_IRQn,
    };

    void I2C::setTiming(Timing timing)
    {
        volatile STM32_I2C_REG_TYPE* regs = &STM32_I2C_REGS(index);
        clockgate_enable(STM32_I2C_CLOCKGATE(index), true);
        regs->TIMINGR.d32 = timing.d32;
        clockgate_enable(STM32_I2C_CLOCKGATE(index), false);
        NVIC_ClearPendingIRQ(i2c_irq[index]);
        NVIC_EnableIRQ(i2c_irq[index]);
    }

    enum ::I2C::Result I2C::txn(const ::I2C::Transaction* txn)
    {
        volatile STM32_I2C_REG_TYPE* regs = &STM32_I2C_REGS(index);
        if (!initialized) setTiming(Timing(0, 0, 0, 0, 0));
        error = false;
        curTxn = txn;
        xfer = 0xff;
        busy = true;
        clockgate_enable(STM32_I2C_CLOCKGATE(index), true);
        union STM32_I2C_REG_TYPE::CR1 CR1 = { 0 };
        CR1.b.TCIE = true;
        CR1.b.TXIE = true;
        CR1.b.RXIE = true;
        CR1.b.STOPIE = true;
        CR1.b.ERRIE = true;
        CR1.b.PE = true;
        regs->CR1.d32 = CR1.d32;
        CR1.d32 = 0;
        CR1.b.PE = false;
        regs->CR2.d32 = CR1.d32;
        start();
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

    void I2C::start()
    {
        volatile STM32_I2C_REG_TYPE* regs = &STM32_I2C_REGS(index);
        xfer++;
        advance();
        union STM32_I2C_REG_TYPE::CR2 CR2 = { 0 };
        if (curTxn->address > 0x77)
        {
            CR2.b.SADD = curTxn->address;
            CR2.b.ADD10 = true;
            CR2.b.HEAD10R = !tx;
        }
        else CR2.b.SADD = curTxn->address << 1;
        CR2.b.RD_WRN = !tx;
        CR2.b.START = true;
        CR2.b.NBYTES = MIN(255, totallen);
        if (totallen > 255) CR2.b.RELOAD = true;
        regs->CR2.d32 = CR2.d32;
    }

    void __attribute__((optimize("-O0"))) I2C::irqHandler()
    {
        volatile STM32_I2C_REG_TYPE* regs = &STM32_I2C_REGS(index);
        union STM32_I2C_REG_TYPE::ISR ISR = { regs->ISR.d32 };
        if (ISR.b.RXNE)
        {
            advanceIfNecessary();
            *buf++ = regs->RXDR;
            len--;
            totallen--;
        }
        else if (ISR.b.TXIS)
        {
            advanceIfNecessary();
            regs->TXDR = *buf++;
            len--;
            totallen--;
        }
        else if (ISR.b.TCR)
        {
            union STM32_I2C_REG_TYPE::CR2 CR2 = { regs->CR2.d32 };
            CR2.b.NBYTES = MIN(255, totallen);
            if (totallen > 255) CR2.b.RELOAD = true;
            regs->CR2.d32 = CR2.d32;
        }
        else if (ISR.b.TC)
        {
            if (!last) start();
            else
            {
                union STM32_I2C_REG_TYPE::CR2 CR2 = { 0 };
                CR2.b.STOP = true;
                regs->CR2.d32 = CR2.d32;
            }
        }
        else if (ISR.b.BERR || ISR.b.OVR || ISR.b.ARLO || ISR.b.TIMEOUT || ISR.b.ALERT || ISR.b.PECERR
              || (ISR.b.STOPF && (totallen || !last)))
        {
            error = true;
            union STM32_I2C_REG_TYPE::CR1 CR1 = { regs->CR1.d32 };
            CR1.b.ERRIE = false;
            regs->CR1.d32 = CR1.d32;
            union STM32_I2C_REG_TYPE::CR2 CR2 = { 0 };
            CR2.b.STOP = true;
            regs->CR2.d32 = CR2.d32;
        }
        if (ISR.b.STOPF)
        {
            busy = false;
            regs->CR1.d32 = 0;
        }
    }

    I2C I2C::I2C1(0);
    I2C I2C::I2C2(1);
}

extern "C" void i2c1_irqhandler()
{
    STM32::I2C::I2C1.irqHandler();
}

extern "C" void i2c2_irqhandler()
{
    STM32::I2C::I2C2.irqHandler();
}
