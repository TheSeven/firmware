#include "global.h"
#include "app/main.h"
#include "sys/time.h"
#include "sys/util.h"
#include "interface/irq/irq.h"
#include "interface/clockgate/clockgate.h"
#include "cpu/arm/cortexm/cmsis.h"
#include "cpu/arm/cortexm/irq.h"
#include "soc/stm32/f0/clockgates.h"
#include "soc/stm32/spi.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/uart.h"
#include "device/nrf/nrf24l01p/nrf24l01p.h"


enum Command
{
    RxPacket = 0x00,
    TxPacket = 0x01,
    TxDone = 0x02,
    TxFlush = 0x03,
    TxFlushed = 0x04,
    ConfigRadio = 0x10,
    ConfigAddr = 0x11,
    ConfigSize = 0x12,
    ConfigRole = 0x13,
    ConfigEnable = 0x14,
    ConfigReset = 0x1f,
};


void transmittedHandler(bool success, int retransmissions);
void receivedHandler(int pipe, uint8_t* data, int length);


NRF::NRF24L01P radio(&STM32::SPI::SPI1, PIN_RADIO_CS, RADIO_FREQ, &receivedHandler, &transmittedHandler);
UART::Bytewise* uart = &STM32::UART::UART1;
UART::Buffered<1024, 16> buf(uart);
uint8_t packet[64];
uint8_t packetPos = 0;
uint8_t discardBytes = 0;


void initRadio()
{
    static uint8_t address[] = { 0xf0, 0x00, 0x00 };
    radio.setTxAddress(address, sizeof(address));
    for (int i = 1; i < 6; i++)
    {
        address[0] = 0xf0 + i;
        radio.setRxAddress(i, address, sizeof(address));
    }
    NRF::NRF24L01P::Configuration nrfConfig(
        NRF::Radio::Config(NRF::Radio::Role_PRX, true, NRF::Radio::CrcMode_16Bit, false, false, false),
        NRF::NRF24L01P::AutoAckCtl(false, false, false, false, false, false),
        NRF::NRF24L01P::RxPipeEnable(false, false, false, false, false, false),
        NRF::NRF24L01P::AddressCtl(NRF::NRF24L01P::Width_24Bit),
        NRF::NRF24L01P::RetransCtl(0, 5),
        NRF::NRF24L01P::RfChannel(82),
        NRF::NRF24L01P::RfSetup(false, NRF::NRF24L01P::Power_0dBm, NRF::NRF24L01P::DataRate_1Mbit, false),
        NRF::NRF24L01P::DynLengthCtl(false, false, false, false, false, false),
        NRF::NRF24L01P::FeatureCtl(false, true, true)
    );
    radio.configure(&nrfConfig);
}


void initUART()
{
    STM32::UART::UART1.configure(1200000, 1000, {STM32::UART::Config::StopBits1_0, STM32::UART::Config::ParityNone, false, false, false});
}


int main()
{
    GPIO::enableFast(PIN_RADIO_IRQ, true);
    initRadio();
    initUART();

    while (true)
    {
        if (!GPIO::getLevel(PIN_RADIO_IRQ)) radio.handleIRQ();

        int av;
        if ((av = buf.readAvailable()))
        {
            if (discardBytes)
            {
                buf.read();
                discardBytes--;
                packetPos = 0;
                continue;
            }
            if (!packetPos)
            {
                *packet = buf.read();
                if (*packet > 63)
                {
                    discardBytes = 64;
                    for (int i = 0; i < 80; i++) uart->bwTx(0xff);
                    continue;
                }
                else if (*packet) packetPos++;
                else uart->bwTx(0);
                continue;
            }
            else if (packetPos < *packet + 1) packetPos += buf.read(&packet[packetPos], MIN(av, *packet - packetPos + 1));
            if (packetPos && packetPos >= *packet + 1)
            {
                uint8_t csum = 0;
                for (int i = 1; i < packetPos; i++) csum += packet[i];
                if (csum)
                {
                    discardBytes = 64;
                    for (int i = 0; i < 80; i++) uart->bwTx(0xff);
                    continue;
                }
                packetPos = 0;
                if (*packet >= 2)
                    switch (packet[1])
                    {
                    case TxPacket:
                    {
                        if (*packet < 3 || *packet > 35) break;
                        NRF::SPI::Status status = radio.getStatus();
                        if (status.b.txFull)
                        {
                            radio.flushTx();
                            uart->bwTx(2);
                            uart->bwTx(TxFlushed);
                            uart->bwTx(-TxFlushed);
                        }
                        else radio.transmit(((int8_t*)packet)[2], &packet[3], *packet - 3);
                        break;
                    }
                    case TxFlush:
                        if (*packet != 2) break;
                        radio.flushTx();
                        uart->bwTx(2);
                        uart->bwTx(TxFlushed);
                        uart->bwTx(-TxFlushed);
                        break;
                    case ConfigRadio:
                    {
                        if (*packet != 11) break;
                        NRF::NRF24L01P::Configuration* cfg = (NRF::NRF24L01P::Configuration*)&packet[2];
                        cfg->config.b.maskDataReceived = false;
                        cfg->config.b.maskDataSent = false;
                        cfg->config.b.maskMaxRetrans = false;
                        radio.configure(cfg);
                        break;
                    }
                    case ConfigAddr:
                        if (*packet < 4 || *packet > 8) break;
                        if (packet[2] & 0x80) radio.setTxAddress(&packet[3], *packet - 3);
                        else radio.setRxAddress(packet[2], &packet[3], *packet - 3);
                        break;
                    case ConfigSize:
                        if (*packet != 3) break;
                        radio.setPacketSize(packet[2] >> 5, packet[2] & 0x3f);
                        break;
                    case ConfigRole:
                        if (*packet != 3) break;
                        radio.setMode(packet[2]);
                        break;
                    case ConfigEnable:
                        if (*packet != 3) break;
                        radio.enablePipe(packet[2] & 0x7, packet[2] >> 7);
                        break;
                    case ConfigReset:
                        if (*packet != 2) break;
                        initRadio();
                        break;
                    }
            }
        }
    }
}


void transmittedHandler(bool success, int retransmissions)
{
    if (!success)
    {
        radio.flushTx();
        uart->bwTx(2);
        uart->bwTx(TxFlushed);
        uart->bwTx(-TxFlushed);
    }
    uint8_t arg = (!success << 7) | retransmissions;
    uart->bwTx(3);
    uart->bwTx(TxDone);
    uart->bwTx(arg);
    uart->bwTx(0 - TxDone - arg);
}


void receivedHandler(int pipe, uint8_t* data, int length)
{
    uart->bwTx(length + 3);
    uart->bwTx(RxPacket);
    uart->bwTx(pipe);
    uint8_t csum = 0 - RxPacket - pipe;
    while (length--)
    {
        csum -= *data;
        uart->bwTx(*data++);
    }
    uart->bwTx(csum);
}
