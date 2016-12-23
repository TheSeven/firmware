#pragma once

#include "global.h"
#include "interface/i2c/i2c.h"


namespace STM32
{
    class __attribute__((packed,aligned(4))) I2C final : public ::I2C::Bus
    {
    public:
        union __attribute__((packed)) Timing
        {
            uint32_t d32;
            struct __attribute__((packed))
            {
                uint32_t SCLL : 8;
                uint32_t SCLH : 8;
                uint32_t SDADEL : 4;
                uint32_t SCLDEL : 4;
                uint32_t : 4;
                uint32_t PRESC : 4;
            } b;
            constexpr Timing(uint32_t prescaler, uint32_t sclLowHold, uint32_t sclHighHold,
                             uint32_t sdaSetup, uint32_t sdaHold)
                : b{sclLowHold, sclHighHold, sdaHold, sdaSetup, prescaler} {}
        };

        union __attribute__((packed)) Timeout
        {
            uint32_t d32;
            struct __attribute__((packed))
            {
                uint32_t TIMEOUTA : 12;
                uint32_t TIDLE : 1;
                uint32_t : 2;
                uint32_t TIMOUTEN : 1;
                uint32_t TIMEOUTB : 12;
                uint32_t : 3;
                uint32_t TEXTEN : 1;
            } b;
            constexpr Timeout(bool singleBitEnable, uint32_t singleBitTimeout,
                              bool cumulativeEnable, uint32_t cumulativeTimeout)
                : b{singleBitTimeout, 0, singleBitEnable, cumulativeTimeout, cumulativeEnable} {}
        };

    private:
        const uint8_t index;
        bool initialized = 0;
        const ::I2C::Transaction* volatile curTxn = NULL;
        volatile bool busy = false;
        volatile ::I2C::Result error = ::I2C::RESULT_OK;
        uint8_t xfer = 0;
        uint8_t* buf = NULL;
        uint32_t totallen = 0;
        uint16_t len = 0;
        bool tx = false;
        bool last = false;

        constexpr I2C(int index) : index(index) {}
        void start();
        void advance();
        void advanceIfNecessary();

    public:
        void setTiming(Timing timing);
        void setTimeout(Timeout timing);
        enum ::I2C::Result txn(const ::I2C::Transaction* txn);
        void irqHandler();

        static I2C I2C1;
        static I2C I2C2;
    };

}

