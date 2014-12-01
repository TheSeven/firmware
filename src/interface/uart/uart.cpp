#include "global.h"
#include "interface/uart/uart.h"
#include "sys/util.h"


UART::BytewiseHandler::BytewiseHandler(Bytewise* uart) : uart(uart)
{
    uart->setHandler(this);
}

void UART::Bytewise::bwTxCallback()
{
    if (handler) handler->bwTxCallback();
}

void UART::Bytewise::bwRxCallback(uint8_t byte, enum Result status)
{
    if (handler) handler->bwRxCallback(byte, status);
}

UART::DMAHandler::DMAHandler(DMA* uart) : uart(uart)
{
    uart->setHandler(this);
}

void UART::DMA::dmaTxCallback()
{
    if (handler) handler->dmaTxCallback();
}

void UART::DMA::dmaRxCallback(int bytesLeft, enum Result status)
{
    if (handler) handler->dmaRxCallback(bytesLeft, status);
}

void UART::SoftwareDMA::dmaTx(const uint8_t* data, int len)
{
    while (txLen)
    {
        enter_critical_section();
        if (txLen) idle();
        leave_critical_section();
    }
    txBuf = data;
    txLen = len;
    uart->bwTx(*txBuf++);
}

void UART::SoftwareDMA::cancelTx()
{
    txLen = 0;
}

void UART::SoftwareDMA::bwTxCallback()
{
    if (--txLen > 0) uart->bwTx(*txBuf++);
    else if (!txLen) dmaTxCallback();
}

void UART::SoftwareDMA::dmaRx(uint8_t* data, int len)
{
    while (rxLen)
    {
        enter_critical_section();
        if (rxLen) idle();
        leave_critical_section();
    }
    rxBuf = data;
    rxLen = len;
}

void UART::SoftwareDMA::cancelRx()
{
    rxLen = 0;
}

void UART::SoftwareDMA::bwRxCallback(uint8_t byte, enum Result status)
{
    if (status != RESULT_OK)
    {
        int bytesLeft = rxLen;
        rxLen = 0;
        dmaRxCallback(bytesLeft, status);
        return;
    }
    if (--rxLen > 0) *rxBuf++ = byte;
    else if (!rxLen) dmaRxCallback(0, RESULT_OK);
}

template<typename RXBuf, typename TXBuf> int UART::Buffered<RXBuf, TXBuf>::readAvailable()
{
    return rxBuf.used();
}

template<typename RXBuf, typename TXBuf> int UART::Buffered<RXBuf, TXBuf>::writeAvailable()
{
    return txBuf.free();
}

template<typename RXBuf, typename TXBuf> bool UART::Buffered<RXBuf, TXBuf>::write(uint8_t byte)
{
    return txBuf.put(&byte);
}

template<typename RXBuf, typename TXBuf> bool UART::Buffered<RXBuf, TXBuf>::read(uint8_t* byte)
{
    return rxBuf.get(byte);
}

template<typename RXBuf, typename TXBuf> int UART::Buffered<RXBuf, TXBuf>::write(const uint8_t* buf, int len)
{
    return txBuf.put(buf, len);
}

template<typename RXBuf, typename TXBuf> int UART::Buffered<RXBuf, TXBuf>::read(uint8_t* buf, int len)
{
    return rxBuf.get(buf, len);
}

template<typename RXBuf, typename TXBuf> void UART::Buffered<RXBuf, TXBuf>::blockingWrite(uint8_t byte)
{
    while (true)
    {
        if (txBuf.write(&byte)) return;
        enter_critical_section();
        if (!txBuf.free()) idle();
        leave_critical_section();
    }
}

template<typename RXBuf, typename TXBuf> void UART::Buffered<RXBuf, TXBuf>::blockingRead(uint8_t* byte)
{
    while (true)
    {
        if (rxBuf.read(byte)) return;
        enter_critical_section();
        if (!rxBuf.used()) idle();
        leave_critical_section();
    }
}

template<typename RXBuf, typename TXBuf> void UART::Buffered<RXBuf, TXBuf>::blockingWrite(const uint8_t* buf, int len)
{
    while (len)
    {
        int written = txBuf.write(buf, len);
        len -= written;
        if (!len) return;
        buf += written;
        enter_critical_section();
        if (!txBuf.free()) idle();
        leave_critical_section();
    }
}

template<typename RXBuf, typename TXBuf> void UART::Buffered<RXBuf, TXBuf>::blockingRead(uint8_t* buf, int len)
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

template<typename RXBuf, typename TXBuf> void UART::Buffered<RXBuf, TXBuf>::bwTxCallback()
{
    uint8_t byte;
    if (txBuf.get(&byte)) uart->bwTx(byte);
}

template<typename RXBuf, typename TXBuf> void UART::Buffered<RXBuf, TXBuf>::bwRxCallback(uint8_t byte, enum Result status)
{
    if (status == RESULT_OK && !rxBuf.put(byte)) status = RESULT_OVERFLOW_ERROR;
    if (status != RESULT_OK) rxErrorCallback(rxBuf.used(), status);
}
