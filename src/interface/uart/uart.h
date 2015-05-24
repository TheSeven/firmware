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

    class __attribute__((packed,aligned(4))) SoftwareDMA : public DMA, public BytewiseHandler
    {
    protected:
        const uint8_t* volatile txBuf;
        uint8_t* volatile rxBuf;
        uint16_t volatile txLen;
        uint16_t volatile rxLen;

    public:
        SoftwareDMA(Bytewise* uart) : DMA(), BytewiseHandler(uart) {}

        void dmaTx(const uint8_t* data, int len);
        void cancelTx();
        void bwTxCallback();
        void dmaRx(uint8_t* data, int len);
        void cancelRx();
        void bwRxCallback(uint8_t byte, enum Result status);
    };

    template<size_t RXBuf, size_t TXBuf> class __attribute__((packed,aligned(4))) Buffered;

    template<size_t RXBuf, size_t TXBuf> class __attribute__((packed,aligned(4))) BufferedHandler
    {
    protected:
        Buffered<RXBuf, TXBuf>* uart;

        BufferedHandler(Buffered<RXBuf, TXBuf>* uart) : uart(uart)
        {
            uart->setHandler(this);
        }

    public:
        virtual void rxErrorCallback(int bytesBefore, enum Result status) = 0;
    };

    template<size_t RXBuf, size_t TXBuf> class __attribute__((packed,aligned(4))) Buffered : public BytewiseHandler
    {
    protected:
        RingBuffer<uint8_t, TXBuf> txBuf;
        RingBuffer<uint8_t, RXBuf> rxBuf;
        BufferedHandler<RXBuf, TXBuf>* handler;

    public:
        Buffered(Bytewise* uart) : BytewiseHandler(uart), handler(NULL) {}

        void setHandler(BufferedHandler<RXBuf, TXBuf>* handler)
        {
            this->handler = handler;
        }

        int readAvailable()
        {
            return rxBuf.used();
        }

        int writeAvailable()
        {
            return txBuf.free();
        }

        uint8_t peek()
        {
            uint8_t data;
            rxBuf.peek(&data);
            return data;
        }

        bool write(uint8_t byte)
        {
            enter_critical_section();
            bool empty = !txBuf.used();
            bool result = txBuf.put(&byte);
            leave_critical_section();
            if (result && empty) bwTxCallback();
            return result;
        }

        uint8_t read()
        {
            uint8_t byte;
            rxBuf.get(&byte);
            return byte;
        }

        bool read(uint8_t* byte)
        {
            return rxBuf.get(byte);
        }

        int write(const uint8_t* buf, int len)
        {
            enter_critical_section();
            bool empty = !txBuf.used();
            bool result = txBuf.put(buf, len);
            leave_critical_section();
            if (result && empty) bwTxCallback();
            return result;
        }

        int read(uint8_t* buf, int len)
        {
            return rxBuf.get(buf, len);
        }

        void blockingWrite(uint8_t byte)
        {
            while (true)
            {
                enter_critical_section();
                bool empty = !txBuf.used();
                bool result = txBuf.write(&byte);
                if (!result) idle();
                leave_critical_section();
                if (result && empty) bwTxCallback();
                if (result) break;
            }
        }

        void blockingRead(uint8_t* byte)
        {
            while (true)
            {
                if (rxBuf.read(byte)) return;
                enter_critical_section();
                if (!rxBuf.used()) idle();
                leave_critical_section();
            }
        }

        void blockingWrite(const uint8_t* buf, int len)
        {
            while (len)
            {
                enter_critical_section();
                bool empty = !txBuf.used();
                int written = txBuf.put(buf, len);
                if (!written) idle();
                leave_critical_section();
                if (written && empty) bwTxCallback();
                len -= written;
                if (!len) return;
                buf += written;
            }
        }

        void blockingRead(uint8_t* buf, int len)
        {
            while (len)
            {
                int read = rxBuf.read(buf, len);
                len -= read;
                if (!len) return;
                buf += read;
                enter_critical_section();
                if (!rxBuf.used()) idle();
                leave_critical_section();
            }
        }

        void bwTxCallback()
        {
            uint8_t byte;
            if (txBuf.get(&byte)) uart->bwTx(byte);
        }

        void bwRxCallback(uint8_t byte, enum Result status)
        {
            if (status == RESULT_OK && !rxBuf.put(&byte)) status = RESULT_OVERFLOW_ERROR;
            if (status != RESULT_OK) rxErrorCallback(rxBuf.used(), status);
        }

        void rxErrorCallback(int bytesBefore, enum Result status)
        {
            if (handler) handler->rxErrorCallback(bytesBefore, status);
        }
    };

}
