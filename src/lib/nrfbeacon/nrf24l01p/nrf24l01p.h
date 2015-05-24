#pragma once

#include "global.h"
#include "../manager.h"
#include "device/nrf/nrf24l01p/nrf24l01p.h"


namespace NRFBeacon
{

    class __attribute__((packed,aligned(4))) NRF24L01P final : public Manager
    {
    public:
        union __attribute__((packed)) RadioSettings
        {
            uint8_t u8[12];
            uint32_t u32[3];
            struct __attribute__((packed))
            {
                union NRF::NRF24L01P::RfChannel rfChannel;
                union NRF::NRF24L01P::RfSetup rfSetup;
                union NRF::NRF24L01P::RetransCtl retransCtl;
                union NRF::NRF24L01P::AddressCtl addressCtl;
                uint8_t addr[5];
            } f;
        };

    protected:
        void applyRadioSettings(void* settings, int len);
        void resetRadioSettings();

    public:
        constexpr NRF24L01P(NRF::Radio* radio, const Identification* id, uint8_t idleInterval, uint8_t maxTimeout)
            : Manager(radio, id, idleInterval, maxTimeout) {}
    };

}
