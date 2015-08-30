#pragma once
#include "global.h"
#include "app/sensornode/sensor.h"
#include "device/htu21d/htu21d.h"


class SensorNodeSensorTemperatureHTU21D : public SensorNodeSensorDriver
{
    HTU21D* sensor;

public:
    SensorNodeSensorTemperatureHTU21D() {}
    SensorNodeSensorTemperatureHTU21D(HTU21D* sensor, uint16_t interval, uint8_t offset)
        : SensorNodeSensorDriver(2, 0x54313248, 0, interval, offset), sensor(sensor) {}
    uint32_t readValue();
};
