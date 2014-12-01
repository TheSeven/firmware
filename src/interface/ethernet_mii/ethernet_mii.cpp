#include "global.h"
#include "interface/ethernet_mii/ethernet_mii.h"
#include "sys/util.h"

enum Ethernet::Result Ethernet::MII::Driver::update(Interface* intf)
{
    enum Result rc;
    if ((rc = phy->sense(intf, this)) != RESULT_OK) return rc;
    return mac->update(intf, this);
}

err_t Ethernet::MII::Driver::lwipInit(Interface* intf)
{
    return mac->lwipInit(intf, this);
}

err_t Ethernet::MII::Driver::lwipTx(struct pbuf *p)
{
    return mac->lwipTx(p);
}
