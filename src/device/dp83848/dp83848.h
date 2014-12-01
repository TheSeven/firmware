#pragma once

#include "global.h"
#include "interface/ethernet_mii/ethernet_mii.h"


class __attribute__((packed,aligned(4))) DP83848 final : public Ethernet::MII::PHY
{
public:
    DP83848(Ethernet::MII::MAC* mac, uint8_t phyId);
    enum Ethernet::Result sense(Ethernet::Interface* intf, Ethernet::MII::Driver* driver);

private:
    Ethernet::MII::MAC* mac;
    uint8_t phyId;
};
