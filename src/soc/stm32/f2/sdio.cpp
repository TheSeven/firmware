#include "global.h"
#include "soc/stm32/f2/sdio.h"
#include "soc/stm32/f2/sdio_regs.h"
#include "soc/stm32/f2/rcc.h"
#include "interface/clockgate/clockgate.h"
#include "interface/storage/storage.h"
#include "sys/util.h"
#include "sys/time.h"

namespace STM32
{

    enum Storage::Result SDIO::configure(int frequency, int width)
    {
        int clkdiv = (RCC::get48MClockFrequency() - 1) / frequency;
        if (clkdiv > 257) return Storage::RESULT_UNSUPPORTED;
        union STM32_SDIO_REG_TYPE::CLKCR CLKCR = { 0 };
        CLKCR.b.CLKDIV = clkdiv - 1;
        CLKCR.b.CLKEN = true;
        CLKCR.b.BYPASS = !clkdiv;
        if (width == 8) CLKCR.b.WIDBUS = 2;
        else if (width == 4) CLKCR.b.WIDBUS = 1;
        else if (width != 1) return Storage::RESULT_UNSUPPORTED;
        clockgate_enable(STM32_SDIO_CLOCKGATE, true);
        STM32_SDIO_REGS.CLKCR.d32 = CLKCR.d32;
        udelay(100000000 / frequency);
        //CLKCR.b.PWRSAV = true;
        //CLKCR.b.HWFC_EN = true;
        STM32_SDIO_REGS.CLKCR.d32 = CLKCR.d32;
        union STM32_SDIO_REG_TYPE::POWER POWER = { 0 };
        POWER.b.PWRCTRL = 3;
        STM32_SDIO_REGS.POWER.d32 = POWER.d32;
        clockgate_enable(STM32_SDIO_CLOCKGATE, false);
        return Storage::RESULT_OK;
    }

    enum Storage::Result SDIO::command(struct Command command, uint32_t arg, void* response)
    {
        union STM32_SDIO_REG_TYPE::CMD CMD = { 0 };
        CMD.b.CMDINDEX = command.cmd;
        if (command.responseType == RESPONSE_SHORT) CMD.b.WAITRESP = 1;
        else if (command.responseType == RESPONSE_LONG) CMD.b.WAITRESP = 3;
        CMD.b.CPSMEN = true;
        bool old = clockgate_enable(STM32_SDIO_CLOCKGATE, true);
        STM32_SDIO_REGS.ARG = arg;
        STM32_SDIO_REGS.CMD.d32 = CMD.d32;
        union STM32_SDIO_REG_TYPE::STA STA;
        do STA.d32 = STM32_SDIO_REGS.STA.d32; while (STA.b.CMDACT);
        uint8_t rcmd = STM32_SDIO_REGS.RESPCMD.b.RESPCMD;
        if (command.responseType == RESPONSE_SHORT) *((uint32_t*)response) = STM32_SDIO_REGS.RESP[0];
        else if (command.responseType == RESPONSE_LONG)
        {
            ((uint32_t*)response)[0] = STM32_SDIO_REGS.RESP[0];
            ((uint32_t*)response)[1] = STM32_SDIO_REGS.RESP[1];
            ((uint32_t*)response)[2] = STM32_SDIO_REGS.RESP[2];
            ((uint32_t*)response)[3] = STM32_SDIO_REGS.RESP[3];
        }
        if (!old)
        {
            STM32_SDIO_REGS.ICR.d32 = STA.d32;
            clockgate_enable(STM32_SDIO_CLOCKGATE, false);
        }
        if (STA.b.CTIMEOUT) return Storage::RESULT_TIMEOUT;
        if (!command.ignoreCRC && STA.b.CCRCFAIL) return Storage::RESULT_COMM_ERROR;
        if (command.responseType != RESPONSE_NONE && !command.ignoreOpcode && rcmd != command.cmd)
            return Storage::RESULT_UNKNOWN_ERROR;
        return Storage::RESULT_OK;
    }

    enum Storage::Result SDIO::startTransfer(void* buf, uint32_t len, struct DataFlags flags)
    {
        buffer = (uint32_t*)buf;
        length = len >> 2;
        dataFlags = flags;
        union STM32_SDIO_REG_TYPE::DCTRL DCTRL = { 0 };
        DCTRL.b.DTEN = true;
        DCTRL.b.DTDIR = flags.direction == DIRECTION_READ;
        DCTRL.b.DTMODE = flags.blockSize == BLOCKSIZE_STREAM;
        DCTRL.b.DBLOCKSIZE = flags.blockSize;
        clockgate_enable(STM32_SDIO_CLOCKGATE, true);
        STM32_SDIO_REGS.DTIMER = 0x1ffff << flags.timeout;
        STM32_SDIO_REGS.DLEN.d32 = len;
        STM32_SDIO_REGS.DCTRL.d32 = DCTRL.d32;
        return Storage::RESULT_RESOURCE_BUSY;
    }

    enum Storage::Result SDIO::getTransferStatus()
    {
        union STM32_SDIO_REG_TYPE::STA STA = { STM32_SDIO_REGS.STA.d32 };
        if (dataFlags.direction == DIRECTION_READ)
            while (length)
            {
                STA.d32 = STM32_SDIO_REGS.STA.d32;
                if (!STA.b.RXDAVL) break;
                *buffer++ = *STM32_SDIO_REGS.FIFO;
                length--;
            }
        else
            while (length)
            {
                STA.d32 = STM32_SDIO_REGS.STA.d32;
                if (STA.b.TXFIFOF) break;
                *STM32_SDIO_REGS.FIFO = *buffer++;
                length--;
            }
        if (STA.b.TXACT || STA.b.RXACT) return Storage::RESULT_RESOURCE_BUSY;
        STM32_SDIO_REGS.ICR.d32 = STA.d32;
        clockgate_enable(STM32_SDIO_CLOCKGATE, false);
        if (STA.b.DTIMEOUT) return Storage::RESULT_TIMEOUT;
        if (!STA.b.DBCKEND) return Storage::RESULT_COMM_ERROR;
        return Storage::RESULT_OK;
    }

    enum Storage::Result SDIO::cancelTransfer()
    {
        union STM32_SDIO_REG_TYPE::STA STA;
        do STA.d32 = STM32_SDIO_REGS.STA.d32;
        while (STA.b.TXACT || STA.b.RXACT);  // TODO: There must be a way to really kill that transfer...
        STM32_SDIO_REGS.ICR.d32 = STA.d32;
        clockgate_enable(STM32_SDIO_CLOCKGATE, false);
        return Storage::RESULT_OK;
    }

}
