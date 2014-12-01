#pragma once

#include "global.h"
#include "interface/ethernet_mii/ethernet_mii.h"


class __attribute__((packed,aligned(4))) LAN8720 final : public Ethernet::MII::PHY
{
public:
    LAN8720(Ethernet::MII::MAC* mac, uint8_t phyId);
    enum Ethernet::Result sense(Ethernet::Interface* intf, Ethernet::MII::Driver* driver);
    enum Ethernet::Result power(bool on);

private:
    Ethernet::MII::MAC* mac;
    uint8_t phyId;
};
