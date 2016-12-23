#pragma once

#include "global.h"
#include "interface/gpio/gpio.h"
#include "interface/spi/spi.h"


#ifndef NRF_OPTIMIZE
#define NRF_OPTIMIZE
#endif


namespace NRF
{

    class __attribute__((packed,aligned(4))) SPI : public ::SPI::Device
    {
    public:
        enum Command
        {
            Cmd_ReadReg = 0x000,
            Cmd_WriteReg = 0x120,
            Cmd_Activate = 0x150,
            Cmd_GetRxSize = 0x060,
            Cmd_ReadPacket = 0x061,
            Cmd_WritePacket = 0x1a0,
            Cmd_WriteACKPayload = 0x1a8,
            Cmd_WriteBroadcastPacket = 0x1b0,
            Cmd_FlushTx = 0x0e1,
            Cmd_FlushRx = 0x0e2,
            Cmd_ReuseTx = 0x0e3,
            Cmd_GetStatus = 0x0ff,
        };

        union __attribute__((packed)) Status
        {
            struct __attribute__((packed)) b
            {
                bool txFull : 1;
                uint8_t rxPipe : 3;
                bool maxRetrans : 1;
                bool dataSent : 1;
                bool dataReceived : 1;
                uint8_t regBank : 1;
            } b;
            uint8_t d8;
            constexpr Status() : b{0, 0, 0, 0, 0, 0} {}
            constexpr Status(uint8_t byte) : d8{byte} {}
            constexpr Status(bool maxRetrans, bool dataSent, bool dataReceived)
                : b{0, 0, maxRetrans, dataSent, dataReceived, 0} {}
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
        bool stayAwake(bool on);
    };

}
