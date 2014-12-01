#pragma once

#include "global.h"


namespace I2C
{

    enum Result
    {
        RESULT_OK = 0,
        RESULT_INVALID_STATE,
        RESULT_INVALID_ARGUMENT,
        RESULT_UNSUPPORTED,
        RESULT_UNKNOWN_ERROR,
        RESULT_NAK,
    };

    class __attribute__((packed,aligned(4))) Transaction final
    {
    public:
        uint32_t address : 10;
        uint32_t reserved : 14;
        uint32_t transferCount : 8;
        class __attribute__((packed,aligned(4))) Transfer final
        {
        public:
            enum __attribute__((packed)) Type
            {
                TYPE_TX,
                TYPE_RX,
                TYPE_CONT,
            } type;
            uint8_t reserved = 0;
            uint16_t len;
            union __attribute__((packed,aligned(4)))
            {
                const void* txbuf;
                void* rxbuf;
            };

            constexpr Transfer(Type type, uint16_t len, const void* buf)
                : type(type), reserved(0), len(len), txbuf(buf) {}
        } transfers[];

        constexpr Transaction(uint32_t address, uint32_t transferCount)
            : address(address), reserved(0), transferCount(transferCount) {}
    };

    class __attribute__((packed,aligned(4))) Bus
    {
    public:
        virtual int setFrequency(int frequency) = 0;
        virtual enum Result txn(const Transaction* txn) = 0;
        enum Result readRegs(int address, int reg, void* buf, int len);
        enum Result writeRegs(int address, int reg, const void* buf, int len);
    };

    class __attribute__((packed,aligned(4))) LLBus : Bus
    {
    public:
        enum Result txn(const Transaction* txn);
        virtual enum Result start() = 0;
        virtual enum Result stop() = 0;
        virtual enum Result tx(const void* buf, int len) = 0;
        virtual enum Result rx(void* buf, int len, bool naklast) = 0;
    };

}
