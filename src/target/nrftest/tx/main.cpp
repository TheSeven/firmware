#include "global.h"
#include "../main.h"
#include "sys/time.h"
#include "sys/util.h"
#include "../radio.h"
#include "../analyzer.h"


void sendPacket()
{
    radio->transmit(-1, packet.byte, sizeof(packet));
    packet.f.sent++;
}


int main()
{
    initAnalyzer("NRF TX tester");
    initRadio();
    radio->setMode(false);
    sendPacket();
    while (true) pollRadio();
}


void transmittedHandler(bool success, int retransmissions)
{
    if (success) packet.f.success++;
    else radio->flushTx();
    packet.f.retrans += retransmissions;
    sendPacket();
    if (success && packet.f.reset) handleReset();
    displayStats();
}


void receivedHandler(int pipe, uint8_t* data, int length)
{
    if (length != sizeof(packet)) return;
    packet.f.acked++;
    RadioPacket* rx = (RadioPacket*)data;
    packet.f.received = rx->f.received;
    if (rx->f.reset) packet.f.reset = true;
}
