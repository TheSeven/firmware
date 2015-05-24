#pragma once

#include "global.h"
#include "interface/gpio/gpio.h"
#include "device/nrf/nrf_spi.h"


namespace NRF
{

    class __attribute__((packed,aligned(4))) Radio : public SPI
    {
    public:
        enum Register
        {
            Reg_Config = 0x00,
            Reg_Status = 0x07,
            Reg_RxPipeAddress0 = 0x0a,
            Reg_RxPipeAddress1 = 0x0b,
            Reg_RxPipeAddress2 = 0x0c,
            Reg_RxPipeAddress3 = 0x0d,
            Reg_RxPipeAddress4 = 0x0e,
            Reg_RxPipeAddress5 = 0x0f,
            Reg_TxAddress = 0x10,
            Reg_RxDataLength0 = 0x11,
            Reg_RxDataLength1 = 0x12,
            Reg_RxDataLength2 = 0x13,
            Reg_RxDataLength3 = 0x14,
            Reg_RxDataLength4 = 0x15,
            Reg_RxDataLength5 = 0x16,
            Reg_FifoStatus = 0x17,
        };

        union __attribute__((packed)) Config
        {
            uint8_t d8;
            struct __attribute__((packed)) b
            {
                enum Role
                {
                    Role_PTX = 0,
                    Role_PRX = 1,
                } role : 1;
                bool powerUp : 1;
                enum CrcMode
                {
                    CrcMode_None = 0,
                    CrcMode_8Bit = 2,
                    CrcMode_16Bit = 3,
                } crcMode : 2;
                bool maskMaxRetrans : 1;
                bool maskDataSent : 1;
                bool maskDataReceived : 1;
                uint8_t : 1;
            } b;
        };

        Radio(const ::SPI::Bus* bus, GPIO::Pin cspin, int frequency)
            : SPI(bus, cspin, frequency), packetTransmitted(NULL), packetReceived(NULL) {}

        Radio(const ::SPI::Bus* bus, GPIO::Pin cspin, int frequency,
                void (*receivedHandler)(int pipe, uint8_t* packet, int length),
                void (*transmittedHandler)(bool success, int retransmissions))
            : SPI(bus, cspin, frequency), packetTransmitted(transmittedHandler), packetReceived(receivedHandler) {}

        SPI::Status setTxAddress(const uint8_t* address, int length);
        SPI::Status setRxAddress(int pipe, const uint8_t* address, int length);
        SPI::Status setMode(bool receive);
        SPI::Status setPacketSize(int pipe, int length);
        SPI::Status transmit(int pipe, const void* data, int length);

        virtual SPI::Status handleIRQ() = 0;

        void (*packetTransmitted)(bool success, int retransmissions);
        void (*packetReceived)(int pipe, uint8_t* packet, int length);
    };

}
