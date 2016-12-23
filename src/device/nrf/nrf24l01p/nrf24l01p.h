#pragma once

#include "global.h"
#include "interface/gpio/gpio.h"
#include "device/nrf/nrf_radio.h"


namespace NRF
{

    class __attribute__((packed,aligned(4))) NRF24L01P final : public Radio
    {
    public:
        enum Width
        {
            Width_24Bit = 1,
            Width_32Bit = 2,
            Width_40Bit = 3,
        };

        enum Power
        {
            Power_m18dBm = 0,
            Power_m12dBm = 1,
            Power_m6dBm = 2,
            Power_0dBm = 3,
        };

        enum DataRate
        {
            DataRate_1Mbit = 0,
            DataRate_2Mbit = 1,
            DataRate_250Kbit = 4,
        };

        enum Register
        {
            Reg_Config = 0x00,
            Reg_AutoAckCtl = 0x01,
            Reg_RxPipeEnable = 0x02,
            Reg_AddressCtl = 0x03,
            Reg_RetransCtl = 0x04,
            Reg_RfChannel = 0x05,
            Reg_RfSetup = 0x06,
            Reg_Status = 0x07,
            Reg_TxObserve = 0x08,
            Reg_EnergyDetect = 0x09,
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
            Reg_DynLengthCtl = 0x1c,
            Reg_FeatureCtl = 0x1d,
        };

        union __attribute__((packed)) AutoAckCtl
        {
            struct __attribute__((packed)) b
            {
                bool pipe0 : 1;
                bool pipe1 : 1;
                bool pipe2 : 1;
                bool pipe3 : 1;
                bool pipe4 : 1;
                bool pipe5 : 1;
                uint8_t : 2;
            } b;
            uint8_t d8;
            constexpr AutoAckCtl() : d8(0) {}
            constexpr AutoAckCtl(uint8_t d8) : d8(d8) {}
            constexpr AutoAckCtl(bool pipe0, bool pipe1, bool pipe2, bool pipe3, bool pipe4, bool pipe5)
                : b{pipe0, pipe1, pipe2, pipe3, pipe4, pipe5} {}
        };

        union __attribute__((packed)) RxPipeEnable
        {
            struct __attribute__((packed)) b
            {
                bool pipe0 : 1;
                bool pipe1 : 1;
                bool pipe2 : 1;
                bool pipe3 : 1;
                bool pipe4 : 1;
                bool pipe5 : 1;
                uint8_t : 2;
            } b;
            uint8_t d8;
            constexpr RxPipeEnable() : d8(0) {}
            constexpr RxPipeEnable(uint8_t d8) : d8(d8) {}
            constexpr RxPipeEnable(bool pipe0, bool pipe1, bool pipe2, bool pipe3, bool pipe4, bool pipe5)
                : b{pipe0, pipe1, pipe2, pipe3, pipe4, pipe5} {}
        };

        union __attribute__((packed)) AddressCtl
        {
            struct __attribute__((packed))
            {
                Width width : 2;
                uint8_t : 6;
            } b;
            uint8_t d8;
            constexpr AddressCtl() : d8(0) {}
            constexpr AddressCtl(uint8_t d8) : d8(d8) {}
            constexpr AddressCtl(Width width) : b{width} {}
        };

        union __attribute__((packed)) RetransCtl
        {
            struct __attribute__((packed)) b
            {
                uint8_t count : 4;
                uint8_t delay : 4;
            } b;
            uint8_t d8;
            constexpr RetransCtl() : d8(0) {}
            constexpr RetransCtl(uint8_t d8) : d8(d8) {}
            constexpr RetransCtl(uint8_t count, uint8_t delay) : b{count, delay} {}
        };

        union __attribute__((packed)) RfChannel
        {
            struct __attribute__((packed)) b
            {
                uint8_t channel : 7;
                uint8_t : 1;
            } b;
            uint8_t d8;
            constexpr RfChannel() : d8(0) {}
            constexpr RfChannel(uint8_t channel) : b{channel} {}
        };

        union __attribute__((packed)) RfSetup
        {
            struct __attribute__((packed)) b
            {
                bool lnaGain : 1;
                Power power : 2;
                DataRate dataRate : 3;
                uint8_t : 1;
                bool sendContinuousCarrier : 1;
            } b;
            uint8_t d8;
            constexpr RfSetup() : d8(0) {}
            constexpr RfSetup(uint8_t d8) : d8(d8) {}
            constexpr RfSetup(bool lnaGain, Power power, DataRate dataRate, bool sendContinuousCarrier)
                : b{lnaGain, power, dataRate, sendContinuousCarrier} {}
        };

        union __attribute__((packed)) TxObserve
        {
            struct __attribute__((packed)) b
            {
                uint8_t retransCount : 4;
                uint8_t lostPackets : 4;
            } b;
            uint8_t d8;
        };

        union __attribute__((packed)) EnergyDetect
        {
            struct __attribute__((packed)) b
            {
                bool energy : 1;
                uint8_t : 7;
            } b;
            uint8_t d8;
        };

        union __attribute__((packed)) FifoStatus
        {
            struct __attribute__((packed)) b
            {
                bool rxEmpty : 1;
                bool rxFull : 1;
                uint8_t : 2;
                bool txEmpty : 1;
                bool txFull : 1;
                bool txReuse : 1;
                uint8_t : 1;
            } b;
            uint8_t d8;
            constexpr FifoStatus() : d8(0) {}
            constexpr FifoStatus(uint8_t d8) : d8(d8) {}
        };

        union __attribute__((packed)) DynLengthCtl
        {
            struct __attribute__((packed)) b
            {
                bool pipe0 : 1;
                bool pipe1 : 1;
                bool pipe2 : 1;
                bool pipe3 : 1;
                bool pipe4 : 1;
                bool pipe5 : 1;
                uint8_t : 2;
            } b;
            uint8_t d8;
            constexpr DynLengthCtl() : d8(0) {}
            constexpr DynLengthCtl(uint8_t d8) : d8(d8) {}
            constexpr DynLengthCtl(bool pipe0, bool pipe1, bool pipe2, bool pipe3, bool pipe4, bool pipe5)
                : b{pipe0, pipe1, pipe2, pipe3, pipe4, pipe5} {}
        };

        union __attribute__((packed)) FeatureCtl
        {
            struct __attribute__((packed)) b
            {
                bool dynAck : 1;
                bool ackPayload : 1;
                bool dynLength : 1;
                uint8_t : 5;
            } b;
            uint8_t d8;
            constexpr FeatureCtl() : d8(0) {}
            constexpr FeatureCtl(uint8_t d8) : d8(d8) {}
            constexpr FeatureCtl(bool dynAck, bool ackPayload, bool dynLength) : b{dynAck, ackPayload, dynLength} {}
        };

        struct __attribute__((packed)) Configuration
        {
            union Config config;
            union AutoAckCtl autoAckCtl;
            union RxPipeEnable rxPipeEnable;
            union AddressCtl addressCtl;
            union RetransCtl retransCtl;
            union RfChannel rfChannel;
            union RfSetup rfSetup;
            union DynLengthCtl dynLengthCtl;
            union FeatureCtl featureCtl;
            constexpr Configuration() {}
            constexpr Configuration(Config config, AutoAckCtl autoAckCtl, RxPipeEnable rxPipeEnable,
                                    AddressCtl addressCtl, RetransCtl retransCtl, RfChannel rfChannel,
                                    RfSetup rfSetup, DynLengthCtl dynLengthCtl, FeatureCtl featureCtl)
                : config(config), autoAckCtl(autoAckCtl), rxPipeEnable(rxPipeEnable), addressCtl(addressCtl),
                  retransCtl(retransCtl), rfChannel(rfChannel), rfSetup(rfSetup), dynLengthCtl(dynLengthCtl),
                  featureCtl(featureCtl) {}
        };

        NRF24L01P(const ::SPI::Bus* bus, GPIO::Pin cspin, int frequency) : Radio(bus, cspin, frequency) {}
        NRF24L01P(const ::SPI::Bus* bus, GPIO::Pin cspin, int frequency,
                  void (*receivedHandler)(int pipe, uint8_t* packet, int length),
                  void (*transmittedHandler)(bool success, int retransmissions))
            : Radio(bus, cspin, frequency, receivedHandler, transmittedHandler) {}

        void configure(const Configuration* config);
        SPI::Status enablePipe(int pipe, bool on);
        SPI::Status handleIRQ();
    };

}
