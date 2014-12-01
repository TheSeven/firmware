#pragma once

#include "global.h"
#include "interface/gpio/gpio.h"
#include "device/ws2811/ws2811.h"
#include "soc/stm32/timer_regs.h"
#include "soc/stm32/f0/dma_regs.h"


namespace WS2811
{

    class __attribute__((packed,aligned(4))) STM32Driver final : public Strip
    {
    public:
        struct Config
        {
            void* dmaBuffer;
            uint8_t timer;
            uint8_t prescaler;
            uint8_t channel;
            uint8_t parallel;
            uint8_t timeSlot;
            uint8_t timeLow;
            uint8_t timeHigh;
            uint8_t pixelsPerIRQ;
            uint8_t dmaController;
            uint8_t dmaChannel;
        };

        STM32Driver(const Config* config);

        void sendFrame(void** data, int pixels);
        bool isSending();
        void cancelSending();
        void handleIrq();

    private:
        volatile STM32_TIM_REG_TYPE* timer;
        uint8_t pixelsPerIRQ;
        uint8_t parallel;
        uint8_t timeLow;
        uint8_t timeHigh;
        volatile STM32_DMA_REG_TYPE* dma;
        volatile STM32_DMA_STREAM_REG_TYPE* dmaStream;
        uint8_t* dmaBuffer;
        uint8_t dmaChannel;
        uint8_t last;
        uint16_t pixelsPending;
        uint8_t** pixelData;

        void fillDmaBuffer(uint8_t* buffer);
    };

}
