#include "global.h"
#include "soc/stm32/flash.h"
#include "soc/stm32/flash_regs.h"
#include "sys/util.h"


#ifdef SOC_STM32F0
#include "soc/stm32/f0/rcc_regs.h"
#endif
#ifdef SOC_STM32F1
#include "soc/stm32/f1/rcc_regs.h"
#endif


#define STM32_FLASH_WAITSTATES (((STM32_AHB_CLOCK) - 1) / (STM32_FLASH_SPEED))
#if STM32_FLASH_WAITSTATES > 15
#error STM32 AHB bus frequency is too high for this operating voltage!
#elif !defined(SOC_STM32F42X) && STM32_FLASH_WAITSTATES > 7
#error STM32 AHB bus frequency is too high for this operating voltage!
#elif defined(SOC_STM32F0) && STM32_FLASH_WAITSTATES > 1
#error STM32 AHB bus frequency is too high for this operating voltage!
#endif


namespace STM32
{

    void FLASH::init()
    {
        // Configure number of waitstates correctly
        union STM32_FLASH_REG_TYPE::ACR ACR = { 0 };
#if defined(SOC_STM32F0) || defined(SOC_STM32F1)
#ifndef STM32_FLASH_DISABLE_PREFETCH
        ACR.b.PRFTBE = true;
#endif
#else
        ACR.b.DCEN = true;
        ACR.b.ICEN = true;
#ifdef STM32_FLASH_PREFETCH
        ACR.b.PRFTEN = true;
#endif
#endif
        ACR.b.LATENCY = STM32_FLASH_WAITSTATES;
        STM32_FLASH_REGS.ACR.d32 = ACR.d32;
    }

#if defined(SOC_STM32F0) || defined(SOC_STM32F1)
    void STM32FLASH_OPTIMIZE FlashDriver::waitIdle()
    {
        while (STM32_FLASH_REGS.SR.b.BSY);
    }

    bool STM32FLASH_OPTIMIZE FlashDriver::enableWrite()
    {
        bool oldHSI = STM32_RCC_REGS.CR.b.HSION;
        if (!oldHSI) STM32_RCC_REGS.CR.b.HSION = true;
        if (!STM32_FLASH_REGS.CR.b.LOCK) return oldHSI;
        STM32_FLASH_REGS.KEYR = 0x45670123;
        STM32_FLASH_REGS.KEYR = 0xCDEF89AB;
        return oldHSI;
    }

    void STM32FLASH_OPTIMIZE FlashDriver::disableWrite(bool oldHSI)
    {
        STM32_RCC_REGS.CR.b.HSION = oldHSI;
        union STM32_FLASH_REG_TYPE::CR CR = { STM32_FLASH_REGS.CR.d32 };
        CR.b.LOCK = true;
        STM32_FLASH_REGS.CR.d32 = CR.d32;
    }

    enum Storage::Result STM32FLASH_OPTIMIZE FlashDriver::reset()
    {
        return RESULT_OK;
    }

    enum Storage::Result STM32FLASH_OPTIMIZE FlashDriver::getStatus()
    {
        return RESULT_OK;
    }

    enum Storage::Result STM32FLASH_OPTIMIZE FlashDriver::read(uint32_t page, uint32_t len, void* buf)
    {
        if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
        if (!len) return RESULT_OK;
        memcpy(buf, (void*)(0x08000000 + page), len);
        return RESULT_OK;
    }

    enum Storage::Result STM32FLASH_OPTIMIZE FlashDriver::write(uint32_t page, uint32_t len, const void* buf)
    {
        if (page >= pageCount || len > pageCount - page || page & (programSize - 1) || len & (programSize - 1))
            return RESULT_INVALID_ARGUMENT;
        Result result = RESULT_OK;
        uint16_t* rptr = (uint16_t*)buf;
        volatile uint16_t* wptr = (volatile uint16_t*)(0x08000000 + page);
        bool oldHSI = enableWrite();
        union STM32_FLASH_REG_TYPE::CR CR = { STM32_FLASH_REGS.CR.d32 };
        union STM32_FLASH_REG_TYPE::CR CRPG = { CR.d32 };
        CRPG.b.PG = true;
        STM32_FLASH_REGS.CR.d32 = CRPG.d32;
        while (len)
        {
            *wptr++ = *rptr++;
            waitIdle();
            if (STM32_FLASH_REGS.SR.b.EOP)
            {
                union STM32_FLASH_REG_TYPE::SR SR = { 0 };
                SR.b.EOP = true;
                STM32_FLASH_REGS.SR.d32 = SR.d32;
            }
            else
            {
                result = RESULT_UNKNOWN_ERROR;
                break;
            }
            len -= programSize;
        }
        STM32_FLASH_REGS.CR.d32 = CR.d32;
        disableWrite(oldHSI);
        return result;
    }

    enum Storage::Result STM32FLASH_OPTIMIZE FlashDriver::erase(uint32_t page, uint32_t len)
    {
        if (page >= pageCount || len > pageCount - page || page & (eraseSize - 1) || len & (eraseSize - 1))
            return RESULT_INVALID_ARGUMENT;
        Result result = RESULT_OK;
        bool oldHSI = enableWrite();
        union STM32_FLASH_REG_TYPE::CR CR = { STM32_FLASH_REGS.CR.d32 };
        union STM32_FLASH_REG_TYPE::CR CRPER = { CR.d32 };
        CRPER.b.PER = true;
        STM32_FLASH_REGS.CR.d32 = CRPER.d32;
        CRPER.b.STRT = true;
        while (len)
        {
            STM32_FLASH_REGS.AR = 0x08000000 + page;
            STM32_FLASH_REGS.CR.d32 = CRPER.d32;
            waitIdle();
            if (STM32_FLASH_REGS.SR.b.EOP)
            {
                union STM32_FLASH_REG_TYPE::SR SR = { 0 };
                SR.b.EOP = true;
                STM32_FLASH_REGS.SR.d32 = SR.d32;
            }
            else
            {
                result = RESULT_UNKNOWN_ERROR;
                break;
            }
            page += eraseSize;
            len -= eraseSize;
        }
        STM32_FLASH_REGS.CR.b.PER = false;
        disableWrite(oldHSI);
        return result;
    }

    FlashDriver Flash{};
#endif
}

