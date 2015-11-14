#pragma once
#include "global.h"
#include "app/sensornode/sensor.h"
#include "app/sensornode/sensor/humidity/si7021.h"
#include "device/si7021/si7021.h"


class SensorNodeSensorTemperatureSI7021 : public SensorNodeSensorDriver
{
    SI7021* sensor;
    SensorNodeSensorHumiditySI7021* humiditySensor;

public:
    SensorNodeSensorTemperatureSI7021() {}
    SensorNodeSensorTemperatureSI7021(SI7021* sensor, SensorNodeSensorHumiditySI7021* humiditySensor,
                                      uint16_t interval, uint8_t offset)
        : SensorNodeSensorDriver(2, 0x54313248, 0, interval, offset), sensor(sensor), humiditySensor(humiditySensor) {}
    uint32_t readValue();
};
