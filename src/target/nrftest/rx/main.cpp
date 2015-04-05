#include "global.h"
#include "../main.h"
#include "sys/time.h"
#include "sys/util.h"
#include "../radio.h"
#include "../analyzer.h"


void sendAck()
{
    radio->transmit(0, packet.byte, sizeof(packet));
}


int main()
{
    initAnalyzer("NRF RX tester");
    initRadio();
    while (true) pollRadio();
}


void transmittedHandler(bool success, int retransmissions)
{
}


void receivedHandler(int pipe, uint8_t* data, int length)
{
    if (length != sizeof(packet)) return;
    packet.f.received++;
    RadioPacket* rx = (RadioPacket*)data;
    packet.f.sent = rx->f.sent;
    packet.f.retrans = rx->f.retrans;
    packet.f.acked = rx->f.acked;
    packet.f.success = rx->f.success;
    if (rx->f.reset) handleReset();
    sendAck();
    displayStats();
}
