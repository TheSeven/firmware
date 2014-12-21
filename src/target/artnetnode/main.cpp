#include "global.h"
#include "app/main.h"
#include "target/artnetnode/configstore.h"
#include "target/artnetnode/network.h"
#include "target/artnetnode/dmx.h"
#include "target/artnetnode/misc.h"
#include "sys/util.h"


struct udp_pcb* artnet_udp;


void artnet_rx(void* dummy, struct udp_pcb* udp, struct pbuf* p, struct ip_addr* addr, u16_t port)
{
    uint8_t buf[18];
    do
    {
        if (pbuf_copy_partial(p, buf, 18, 0) != 18) break;
        if (memcmp(buf, "Art-Net\0\0\x50", 10)) break;
        uint16_t universe = (buf[15] << 8) | buf[14];
        uint16_t length = (buf[16] << 8) | buf[17];
        if (length > 512) break;
        if (universe >= ARRAYLEN(dmxOutChannel)) break;
        uint8_t* data = dmxOutChannel[universe]->getOutDataPtr();
        if (pbuf_copy_partial(p, data, length, 18) != length) break;
        dmxOutChannel[universe]->sendPacket();
    }
    while (false);
    pbuf_free(p);
}

int main()
{
    earlyInit();
    initConfig();
    initDMX();
    initNetwork();
    lateInit();

    dhcp_start(&netIf.lwipIf);

    artnet_udp = udp_new();
    udp_bind(artnet_udp, IP_ADDR_ANY, 0x1936);
    udp_recv(artnet_udp, &artnet_rx, NULL);

    while (true) doBackgroundWork();
}

void dmxInUpdated(int channel)
{

}
