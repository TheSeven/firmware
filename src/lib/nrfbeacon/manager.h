#pragma once

#include "global.h"
#include "device/nrf/nrf24l01p/nrf24l01p.h"


namespace NRFBeacon
{
    struct __attribute__((packed)) Identification
    {
        struct __attribute__((packed)) Hardware
        {
            uint32_t vendor;
            uint32_t product;
            uint32_t serial;
        } hw;
        struct __attribute__((packed)) Protocol
        {
            uint32_t vendor;
            uint16_t type;
            uint16_t version;
        } protocol;
    };

    enum MessageType
    {
        MessageTypeCommand = 0,
        MessageTypePayloadBeacon = 0xfe,
        MessageTypeBeacon = 0xff,
    };

    enum CommandCode
    {
        CommandSendId = 0,
        CommandSetParameters = 1,
    };

    union __attribute__((packed)) Message
    {
        uint8_t u8[32];
        uint32_t u32[8];
        struct __attribute__((packed))
        {
            uint16_t protocol;
            enum MessageType msgType : 8;
            union __attribute__((packed))
            {
                struct __attribute__((packed))
                {
                    enum CommandCode command : 8;
                    union __attribute__((packed))
                    {
                        uint8_t u8[28];
                        uint32_t u32[7];
                        struct __attribute__((packed))
                        {
                            Identification::Hardware target;
                            uint16_t beaconInterval;
                            uint8_t timeout;
                            uint8_t localId;
                            uint8_t radioSettings[12];
                        } setParameters;
                    } args;
                } cmd;
                struct __attribute__((packed))
                {
                    uint8_t localId;
                    union __attribute__((packed))
                    {
                        uint8_t u8[28];
                        uint32_t u32[7];
                    } payload;
                } beacon;
            } msg;
        };
    };

    class __attribute__((packed,aligned(4))) Manager
    {
    protected:
        NRF::Radio* radio;
        const Identification* id;
        uint8_t idleInterval;  // seconds
        uint8_t maxTimeout;  // seconds
        uint8_t localId;

    public:
        uint8_t payloadLen;
        const void* payload;
        uint16_t interval;  // 10ms steps
        uint8_t timeout;  // seconds

    private:
        bool sendId;

    protected:
        virtual void applyRadioSettings(void* settings, int len) = 0;
        virtual void resetRadioSettings() = 0;

    public:
        constexpr Manager(NRF::Radio* radio, const Identification* id, uint8_t idleInterval, uint8_t maxTimeout)
            : radio(radio), id(id), idleInterval(idleInterval), maxTimeout(maxTimeout), localId(0),
              payloadLen(0), payload(NULL), interval(0), timeout(0), sendId(false) {}
        bool processPacket(void* data, int len);
        bool sendBeacon();
        void timeoutExpired();
    };

}
