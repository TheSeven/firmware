#pragma once

#include "global.h"
#include "interface/i2c/i2c.h"


namespace STM32
{

    class __attribute__((packed,aligned(4))) I2C final : public ::I2C::Bus
    {
    private:
        const uint8_t index;
        bool initialized = 0;
        const ::I2C::Transaction* volatile curTxn = NULL;
        volatile bool busy = false;
        volatile bool error = false;
        volatile bool first = false;
        uint8_t xfer = 0;
        uint8_t* buf = NULL;
        uint32_t totallen = 0;
        uint16_t len = 0;
        bool tx = false;
        bool last = false;

        constexpr I2C(int index) : index(index) {}
        void advance();
        void advanceIfNecessary();

    public:
        int setFrequency(int frequency);
        enum ::I2C::Result txn(const ::I2C::Transaction* txn);
        void ev_handler();
        void er_handler();

        static I2C I2C1;
        static I2C I2C2;
        static I2C I2C3;
    };

}

