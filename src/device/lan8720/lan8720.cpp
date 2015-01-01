#include "global.h"
#include "device/lan8720/lan8720.h"
#include "sys/time.h"
#include "sys/util.h"

LAN8720::LAN8720(Ethernet::MII::MAC* mac, uint8_t phyId) : mac(mac), phyId(phyId)
{
    while (read_usec_timer() < 10000);
    mac->phyWrite(phyId, 0x12, 0x40e0);
    uint16_t data = 0x8000;
    mac->phyWrite(phyId, 0x00, data);
    while (data & 0x8000) mac->phyRead(phyId, 0, &data);
    mac->phyWrite(phyId, 0x11, 0x2000);
}

enum Ethernet::Result LAN8720::power(bool on)
{
    return mac->phyWrite(phyId, 0x00, on ? 0x1000 : 0x0c00);
}

enum Ethernet::Result LAN8720::sense(Ethernet::Interface* intf, Ethernet::MII::Driver* driver)
{
    Ethernet::Result rc;
    uint16_t data;
    if ((rc = mac->phyRead(phyId, 0x1b, &data)) != Ethernet::RESULT_OK) return rc;
    enum Ethernet::MII::Wiring wiring = (data & 0x2000) ? Ethernet::MII::WIRING_MDIX : Ethernet::MII::WIRING_MDI;
    if (wiring != driver->wiring) intf->stateChanged = true;
    driver->wiring = wiring;
    if ((rc = mac->phyRead(phyId, 0x1f, &data)) != Ethernet::RESULT_OK) return rc;
    enum Ethernet::MII::Duplex duplex = (data & 0x0010) ? Ethernet::MII::DUPLEX_FDX : Ethernet::MII::DUPLEX_HDX;
    if (duplex != driver->duplex) intf->stateChanged = true;
    driver->duplex = duplex;
    enum Ethernet::MII::Speed speed = (data & 0x0004) ? Ethernet::MII::SPEED_10 : Ethernet::MII::SPEED_100;
    if (speed != driver->speed) intf->stateChanged = true;
    driver->speed = speed;
    if ((rc = mac->phyRead(phyId, 0x01, &data)) != Ethernet::RESULT_OK) return rc;
    bool linkUp = !!(data & 0x0004);
    if (linkUp != intf->linkUp) intf->stateChanged = true;
    intf->linkUp = linkUp;
    return Ethernet::RESULT_OK;
}
