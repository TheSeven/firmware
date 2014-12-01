#include "global.h"
#include "device/dp83848/dp83848.h"
#include "sys/util.h"


DP83848::DP83848(Ethernet::MII::MAC* mac, uint8_t phyId) : mac(mac), phyId(phyId)
{
    uint16_t data = 0x8000;
    mac->phyWrite(phyId, 0x00, data);
    while (data & 0x8000) mac->phyRead(phyId, 0, &data);
    mac->phyWrite(phyId, 0x11, 0x0003);
    mac->phyWrite(phyId, 0x12, 0x0038);
    mac->phyWrite(phyId, 0x17, 0x0020);
    mac->phyWrite(phyId, 0x19, 0x8020 | phyId);
    mac->phyWrite(phyId, 0x1a, 0x0804);
}

enum Ethernet::Result DP83848::sense(Ethernet::Interface* intf, Ethernet::MII::Driver* driver)
{
    uint16_t data;
    mac->phyRead(phyId, 0x10, &data);
    enum Ethernet::MII::Wiring wiring = (data & 0x4000) ? Ethernet::MII::WIRING_MDIX : Ethernet::MII::WIRING_MDI;
    if (wiring != driver->wiring) intf->stateChanged = true;
    driver->wiring = wiring;
    enum Ethernet::MII::Duplex duplex = (data & 0x0004) ? Ethernet::MII::DUPLEX_FDX : Ethernet::MII::DUPLEX_HDX;
    if (duplex != driver->duplex) intf->stateChanged = true;
    driver->duplex = duplex;
    enum Ethernet::MII::Speed speed = (data & 0x0002) ? Ethernet::MII::SPEED_10 : Ethernet::MII::SPEED_100;
    if (speed != driver->speed) intf->stateChanged = true;
    driver->speed = speed;
    bool linkUp = data & 0x0001;
    if (linkUp != intf->linkUp) intf->stateChanged = true;
    intf->linkUp = linkUp;
    if (data & 0x0080) mac->phyRead(phyId, 0x12, &data);
    return Ethernet::RESULT_OK;
}
