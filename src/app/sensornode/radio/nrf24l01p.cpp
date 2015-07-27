#include "global.h"
#include "app/sensornode/radio.h"
#include "app/sensornode/common.h"
#include "sys/time.h"
#include "sys/util.h"
#include "interface/gpio/gpio.h"
#include "device/nrf/nrf24l01p/nrf24l01p.h"
#include "lib/nrfbeacon/nrf24l01p/nrf24l01p.h"


class SensorNodeRadioNRF24L01P : public SensorNodeRadioDriver
{
    NRF::NRF24L01P::Config config;
    NRF::NRF24L01P nrfRadio;
    NRFBeacon::NRF24L01P nrfBeacon;

public:
    SensorNodeRadioNRF24L01P() : SensorNodeRadioDriver(&nrfRadio, &nrfBeacon),
                                 nrfRadio(SENSORNODE_RADIO_NRF24L01P_SPIBUS, SENSORNODE_RADIO_NRF24L01P_CSPIN,
                                          SENSORNODE_RADIO_NRF24L01P_SPIFREQ, &receivedHandler, &transmittedHandler),
                                 nrfBeacon(&nrfRadio, &deviceId, 60, 60) {}
    void sleep();
    void wake();
    bool detectIRQ();
};

SensorNodeRadioNRF24L01P nrfRadioDriver;
SensorNodeRadioDriver* radioDriver = &nrfRadioDriver;


void SENSORNODE_RADIO_OPTIMIZE SensorNodeRadioNRF24L01P::sleep()
{
    NRF::NRF24L01P::Config nrfOff;
    nrfRadio.readReg(NRF::NRF24L01P::Reg_Config, &config, sizeof(config));
    nrfOff.d8 = config.d8;
    nrfOff.b.powerUp = false;
    nrfRadio.writeReg(NRF::NRF24L01P::Reg_Config, &nrfOff, sizeof(nrfOff));
}

void SENSORNODE_RADIO_OPTIMIZE SensorNodeRadioNRF24L01P::wake()
{
    nrfRadio.writeReg(NRF::NRF24L01P::Reg_Config, &config, sizeof(config));
}

bool SENSORNODE_RADIO_OPTIMIZE SensorNodeRadioNRF24L01P::detectIRQ()
{
    return !GPIO::getLevel(SENSORNODE_RADIO_NRF24L01P_IRQPIN);
}
