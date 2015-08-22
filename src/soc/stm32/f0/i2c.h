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

    private:
        const uint8_t index;
        bool initialized = 0;
        const ::I2C::Transaction* volatile curTxn = NULL;
        volatile bool busy = false;
        volatile bool error = false;
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
        enum ::I2C::Result txn(const ::I2C::Transaction* txn);
        void irqHandler();

        static I2C I2C1;
        static I2C I2C2;
    };

}

