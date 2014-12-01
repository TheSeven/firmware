#include "global.h"
#include "interface/ethernet/ethernet.h"
#include "sys/util.h"


bool Ethernet::Interface::lwipInitialized = false;

Ethernet::Interface::Interface(Driver* driver, uint16_t mtu, uint8_t* macaddr)
    : driver(driver), linkUp(false), stateChanged(true)
{
    if (!lwipInitialized)
    {
        lwip_init();
        lwipInitialized = true;
    }
    memset(&lwipIf, 0, sizeof(lwipIf));
    lwipIf.name[0] = 'n';
    lwipIf.name[1] = 'e';
    lwipIf.output = etharp_output;
    lwipIf.linkoutput = &lwipTx;
    lwipIf.mtu = mtu;
    lwipIf.hwaddr_len = ETHARP_HWADDR_LEN;
    memcpy(lwipIf.hwaddr, macaddr, ETHARP_HWADDR_LEN);
    netif_add(&lwipIf, NULL, NULL, NULL, (void*)this, &initNetIf, &ethernet_input);
    tick();
}

err_t Ethernet::Interface::initNetIf(struct netif *netif)
{
    Interface* intf = (Interface*)netif->state;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
    return intf->driver->lwipInit(intf);
}

err_t Ethernet::Interface::lwipTx(struct netif *netif, struct pbuf *p)
{
    return ((Interface*)netif->state)->driver->lwipTx(p);
}

enum Ethernet::Result Ethernet::Interface::tick()
{
    enum Result rc;
    if ((rc = driver->update(this)) != RESULT_OK) return rc;
    if (stateChanged)
    {
        if (linkUp) netif_set_link_up(&lwipIf);
        else netif_set_link_down(&lwipIf);
        stateChanged = false;
    }
    sys_check_timeouts();
    return RESULT_OK;
}
