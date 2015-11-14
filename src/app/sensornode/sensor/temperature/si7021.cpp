#include "global.h"
#include "app/sensornode/sensor/temperature/si7021.h"
#include "app/sensornode/common.h"


uint32_t SENSORNODE_SENSOR_OPTIMIZE SensorNodeSensorTemperatureSI7021::readValue()
{
    if (humiditySensor && humiditySensor->lastReadTime == now) return sensor->readRawLastTemperature();
    return sensor->readRawTemperature();
}
