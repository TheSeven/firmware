#pragma once

#include "global.h"
#include "lib/ringbuffer/ringbuffer.h"


namespace UART
{

    enum Result
    {
        RESULT_OK = 0,
        UESULT_UNKNOWN_ERROR,
        RESULT_UNSUPPORTED,
        RESULT_NOT_AVAILABLE,
        RESULT_INVALID_ARGUMENT,
        RESULT_OVERFLOW_ERROR,
        RESULT_FRAMING_ERROR,
        RESULT_PARITY_ERROR,
        RESULT_NOISE_ERROR,
    };

    class __attribute__((packed,aligned(4))) Bytewise;

    class __attribute__((packed,aligned(4))) BytewiseHandler
    {
    protected:
        Bytewise* uart;

        BytewiseHandler(Bytewise* uart);

    public:
        virtual void bwTxCallback() = 0;
        virtual void bwRxCallback(uint8_t byte, enum Result status) = 0;
    };

    class __attribute__((packed,aligned(4))) Bytewise
    {
    protected:
        BytewiseHandler* handler;

        void bwTxCallback();
        void bwRxCallback(uint8_t byte, enum Result status);

    public:
        constexpr Bytewise() : handler(NULL) {}

        virtual void bwTx(uint8_t byte) = 0;

        void setHandler(BytewiseHandler* handler)
        {
            this->handler = handler;
        }
    };

    class __attribute__((packed,aligned(4))) DMA;

    class __attribute__((packed,aligned(4))) DMAHandler
    {
    protected:
        DMA* uart;

        DMAHandler(DMA* uart);

    public:
        virtual void dmaTxCallback() = 0;
        virtual void dmaRxCallback(int bytesLeft, enum Result status) = 0;
    };

    class __attribute__((packed,aligned(4))) DMA
    {
    protected:
        DMAHandler* handler;

        void dmaTxCallback();
        void dmaRxCallback(int bytesLeft, enum Result status);

    public:
        constexpr DMA() : handler(NULL) {}

        virtual void dmaTx(const uint8_t* data, int len) = 0;
        virtual void cancelTx() = 0;
        virtual void dmaRx(uint8_t* data, int len) = 0;
        virtual void cancelRx() = 0;

        void setHandler(DMAHandler* handler)
        {
            this->handler = handler;
        }
    };

    class __attribute__((packed,aligned(4))) SoftwareDMA : public DMA, BytewiseHandler
    {
    protected:
        const uint8_t* volatile txBuf;
        uint8_t* volatile rxBuf;
        uint16_t volatile txLen;
        uint16_t volatile rxLen;

    public:
        void dmaTx(const uint8_t* data, int len);
        void cancelTx();
        void bwTxCallback();
        void dmaRx(uint8_t* data, int len);
        void cancelRx();
        void bwRxCallback(uint8_t byte, enum Result status);
    };

    template<typename RXBuf, typename TXBuf> class __attribute__((packed,aligned(4))) Buffered : public BytewiseHandler
    {
    protected:
        RingBuffer<TXBuf> txBuf;
        RingBuffer<RXBuf> rxBuf;

    public:
        int readAvailable();
        int writeAvailable();
        bool write(uint8_t byte);
        bool read(uint8_t* byte);
        int write(const uint8_t* buf, int len);
        int read(uint8_t* buf, int len);
        void blockingWrite(uint8_t byte);
        void blockingRead(uint8_t* byte);
        void blockingWrite(const uint8_t* buf, int len);
        void blockingRead(uint8_t* buf, int len);
        void bwTxCallback();
        void bwRxCallback(uint8_t byte, enum Result status);
        virtual void rxErrorCallback(int bytesBefore, enum Result status) = 0;
    };

}
