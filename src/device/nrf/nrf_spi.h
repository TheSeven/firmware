#pragma once

#include "global.h"
#include "interface/gpio/gpio.h"
#include "interface/spi/spi.h"


#ifndef NRF_OPTIMIZE
#define NRF_OPTIMIZE
#endif


namespace NRF
{

    class __attribute__((packed,aligned(4))) SPI : ::SPI::Device
    {
    public:
        union __attribute__((packed)) Status
        {
            uint8_t d8;
            struct __attribute__((packed)) b
            {
                bool txFull : 1;
                uint8_t rxPipe : 3;
                bool maxRetrans : 1;
                bool dataSent : 1;
                bool dataReceived : 1;
                uint8_t regBank : 1;
            } b;
        };

        SPI(const ::SPI::Bus* bus, GPIO::Pin cspin, int frequency) : Device(bus, cspin, frequency) {}

        Status access(uint32_t command, void* data, int len);
        Status readReg(int reg, void* data, int len);
        Status writeReg(int reg, const void* data, int len);
        Status readPacket(void* data, int len);
        Status writePacket(const void* data, int len);
        Status flushTx();
        Status flushRx();
        Status reuseTxPacket();
        Status getRxPacketSize(uint8_t* size);
        Status writeAckPayload(int pipe, const void* data, int len);
        Status writePacketNoAck(const void* data, int len);
        Status getStatus();
        Status activate(uint8_t feature);

    protected:
        bool stayAwake(bool on);
    };

}
