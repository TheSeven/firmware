#include "global.h"
#include "radio.h"
#include "sys/time.h"
#include "sys/util.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/spi.h"
#include "device/nrf/nrf24l01p/nrf24l01p.h"
#include "main.h"


NRF::NRF24L01P nrf(&STM32::SPI::SPI1, PIN_A1, 10000000, &receivedHandler, &transmittedHandler);
NRF::Radio* radio = &nrf;


void initRadio()
{
    static const uint8_t address[] = { 0x34, 0x43, 0x10, 0x20 };
    nrf.setTxAddress(address, sizeof(address));
    NRF::NRF24L01P::Configuration nrfConfig = { 0 };
    nrfConfig.config.b.powerUp = true;
    nrfConfig.config.b.role = nrfConfig.config.b.Role_PRX;
    nrfConfig.config.b.crcMode = nrfConfig.config.b.CrcMode_16Bit;
    nrfConfig.addressCtl.b.width = nrfConfig.addressCtl.b.Width_32Bit;
    nrfConfig.retransCtl.b.count = 0;
    nrfConfig.retransCtl.b.delay = 5;
    nrfConfig.rfChannel.b.channel = 82;
    nrfConfig.rfSetup.b.power = nrfConfig.rfSetup.b.Power_0dBm;
    nrfConfig.rfSetup.b.dataRate = nrfConfig.rfSetup.b.DataRate_250Kbit;
    nrfConfig.featureCtl.b.ackPayload = true;
    nrfConfig.featureCtl.b.dynLength = true;
    nrfConfig.rxPipeEnable.b.pipe0 = true;
    nrfConfig.autoAckCtl.b.pipe0 = true;
    nrfConfig.dynLengthCtl.b.pipe0 = true;
    nrf.configure(&nrfConfig);
}


void pollRadio()
{
    if (!GPIO::getLevel(PIN_A0)) nrf.handleIRQ();
}
