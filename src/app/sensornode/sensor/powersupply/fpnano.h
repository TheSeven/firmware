#pragma once
#include "global.h"
#include "app/sensornode/sensor.h"


class SensorNodeSensorPowerSupplyFPNano : public SensorNodeSensorDriver
{
public:
    SensorNodeSensorPowerSupplyFPNano() {}
    SensorNodeSensorPowerSupplyFPNano(uint16_t interval, uint8_t offset)
        : SensorNodeSensorDriver(0x76427653, 4, 0, interval, offset) {}
    uint32_t readValue();
};
