#pragma once

#include "global.h"
#include "target/artnetnode/dmx.h"
#include "soc/stm32/uart.h"
#include "soc/stm32/uart_regs.h"

#if defined(SOC_STM32F1)
#include "soc/stm32/f1/gpio.h"
#include "soc/stm32/f1/dma_regs.h"
#else
#include "soc/stm32/gpio.h"
#include "soc/stm32/dma_regs.h"
#endif

enum ChannelMode
{
    ChannelModeDisabled = 0,
    ChannelModeInput = 1,
    ChannelModeOutput = 2,
    ChannelModeBidirectional = ChannelModeInput | ChannelModeOutput,
};


class __attribute__((packed,aligned(4))) DMXBidiChannel : public DMXInChannel, public DMXOutChannel
{
public:
    virtual void setMode(enum ChannelMode mode) = 0;
};

class __attribute__((packed,aligned(4))) DMAStream
{
public:
    uint8_t channel : 3;
    uint8_t stream : 3;
    uint8_t controller : 1;

    constexpr DMAStream(int controller, int stream, int channel)
        : channel(channel), stream(stream), controller(controller) {}
};

class __attribute__((packed,aligned(4))) DMXDMAChannel final : public DMXBidiChannel
{
public:
    DMXDMAChannel(int index, int uart, GPIO::Pin txPin, GPIO::Pin oePin, bool halfDuplex,
                  DMAStream dmaIn, DMAStream dmaOut);

    const uint8_t* getInDataPtr();

    bool isBusy();
    uint8_t* getOutDataPtr();
    void sendPacket();

    void setMode(enum ChannelMode mode);
    void handleUARTIRQ();
    void handleDMAInIRQ();
    void handleDMAOutIRQ();

private:
    uint8_t inData[2][516];
    uint8_t outData[2][512];
    volatile STM32_USART_REG_TYPE* uartRegs;
    volatile STM32_DMA_STREAM_REG_TYPE* dmaRegsIn;
    volatile STM32_DMA_STREAM_REG_TYPE* dmaRegsOut;
    GPIO::Pin txPin;
    GPIO::Pin oePin;
    DMAStream dmaIn;
    DMAStream dmaOut;
    enum ChannelMode mode;
    uint8_t dmaInIRQ;
    uint8_t dmaOutIRQ;
    uint8_t activeInBuf;
    uint8_t activeOutBuf;
    uint8_t index;
    bool outPending;
    bool inBusy;
    bool outBusy;
};


extern DMXBidiChannel* dmxChannel[DMX_IN_CHANNELS];


extern void initDMX();
