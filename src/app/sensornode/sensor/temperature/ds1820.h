#pragma once
#include "global.h"
#include "app/sensornode/sensor.h"
#include "device/ds1820/ds1820.h"


class SensorNodeSensorTemperatureDS1820 : public SensorNodeSensorDriver
{
    DS1820 ds1820;

public:
    SensorNodeSensorTemperatureDS1820() {}
    SensorNodeSensorTemperatureDS1820(OneWire::Bus* bus, uint64_t deviceId, uint16_t interval, uint8_t offset);
    uint32_t readValue();
};
