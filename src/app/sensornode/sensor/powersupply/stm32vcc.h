#pragma once
#include "global.h"
#include "app/sensornode/sensor.h"


class SensorNodeSensorPowerSupplySTM32VCC : public SensorNodeSensorDriver
{
public:
    SensorNodeSensorPowerSupplySTM32VCC() {}
    SensorNodeSensorPowerSupplySTM32VCC(uint16_t interval, uint8_t offset)
        : SensorNodeSensorDriver(0x566d4441, 2, 0, interval, offset) {}
    uint32_t readValue();
#ifdef SENSORNODE_SENSOR_POWERSUPPLY_STM32VCC_AUXCHANNEL
    uint16_t readAux();
#endif

private:
#ifdef SENSORNODE_SENSOR_POWERSUPPLY_STM32VCC_AUXCHANNEL
    int lastTime;
    uint16_t auxData;
#endif
};
