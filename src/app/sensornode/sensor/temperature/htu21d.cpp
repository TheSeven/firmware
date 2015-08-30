#include "global.h"
#include "app/sensornode/sensor/temperature/htu21d.h"


uint32_t SENSORNODE_SENSOR_OPTIMIZE SensorNodeSensorTemperatureHTU21D::readValue()
{
    return sensor->readRawTemperature();
}
