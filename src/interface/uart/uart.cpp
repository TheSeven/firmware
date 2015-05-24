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
