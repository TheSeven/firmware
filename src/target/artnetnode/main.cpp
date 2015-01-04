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
    struct pbuf* buf = pbuf_alloc(PBUF_TRANSPORT, 18, PBUF_RAM);
    if (!buf) return;
    struct pbuf* payload = pbuf_alloc(PBUF_RAW, 512, PBUF_POOL);
    if (!payload)
    {
        pbuf_free(buf);
        return;
    }
    pbuf_take(payload, dmxInChannel[channel]->getInDataPtr(), 512);
    uint8_t* header = (uint8_t*)buf->payload;
    memcpy(header, "Art-Net\0\0\x50\0\0\0", 14);
    header[13] = channel;
    header[14] = 8 + channel;
    header[15] = 0;
    header[16] = 2;
    header[17] = 0;
    pbuf_cat(buf, payload);
    udp_sendto_if(artnet_udp, buf, IP_ADDR_BROADCAST, 0x1936, &netIf.lwipIf);
    pbuf_free(buf);
}
