#include "global.h"
#include "lib/nrfbeacon/nrf24l01p/nrf24l01p.h"
#include "device/nrf/nrf24l01p/nrf24l01p.h"


void NRFBEACON_OPTIMIZE NRFBeacon::NRF24L01P::applyRadioSettings(void* settings, int len)
{
    NRF::NRF24L01P* radio = (NRF::NRF24L01P*)this->radio;
    RadioSettings* data = (RadioSettings*)settings;
    if ((uint32_t)len < sizeof(data->f) - sizeof(data->f.addr)) return;
    if ((uint32_t)len > sizeof(data->f) - sizeof(data->f.addr))
        radio->setTxAddress(data->f.addr, len - sizeof(data->f) + sizeof(data->f.addr));
    NRF::NRF24L01P::Configuration nrfConfig = { 0 };
    nrfConfig.config.b.powerUp = true;
    nrfConfig.config.b.role = nrfConfig.config.b.Role_PTX;
    nrfConfig.config.b.crcMode = nrfConfig.config.b.CrcMode_16Bit;
    nrfConfig.addressCtl.d8 = data->f.addressCtl.d8;
    nrfConfig.retransCtl.d8 = data->f.retransCtl.d8;
    nrfConfig.rfChannel.d8 = data->f.rfChannel.d8;
    nrfConfig.rfSetup.d8 = data->f.rfSetup.d8;
    nrfConfig.featureCtl.b.ackPayload = true;
    nrfConfig.featureCtl.b.dynLength = true;
    nrfConfig.rxPipeEnable.b.pipe0 = true;
    nrfConfig.autoAckCtl.b.pipe0 = true;
    nrfConfig.dynLengthCtl.b.pipe0 = true;
    radio->configure(&nrfConfig);
}

void NRFBEACON_OPTIMIZE NRFBeacon::NRF24L01P::resetRadioSettings()
{
    NRF::NRF24L01P* radio = (NRF::NRF24L01P*)this->radio;
    static const uint8_t address[] = { 0xDD, 0x37, 0xC4 };
    radio->setTxAddress(address, sizeof(address));
    NRF::NRF24L01P::Configuration nrfConfig = { 0 };
    nrfConfig.config.b.powerUp = true;
    nrfConfig.config.b.role = nrfConfig.config.b.Role_PTX;
    nrfConfig.config.b.crcMode = nrfConfig.config.b.CrcMode_16Bit;
    nrfConfig.addressCtl.b.width = nrfConfig.addressCtl.b.Width_24Bit;
    nrfConfig.retransCtl.b.count = 1;
    nrfConfig.retransCtl.b.delay = 2;
    nrfConfig.rfChannel.b.channel = 82;
    nrfConfig.rfSetup.b.power = nrfConfig.rfSetup.b.Power_0dBm;
    nrfConfig.rfSetup.b.dataRate = nrfConfig.rfSetup.b.DataRate_1Mbit;
    nrfConfig.featureCtl.b.ackPayload = true;
    nrfConfig.featureCtl.b.dynLength = true;
    nrfConfig.rxPipeEnable.b.pipe0 = true;
    nrfConfig.autoAckCtl.b.pipe0 = true;
    nrfConfig.dynLengthCtl.b.pipe0 = true;
    radio->configure(&nrfConfig);
}
