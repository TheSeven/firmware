#include "global.h"
#include "app/sensornode/sensor/temperature/htu21d.h"


SENSORNODE_SENSOR_OPTIMIZE SensorNodeSensorTemperatureHTU21D::SensorNodeSensorTemperatureHTU21D(HTU21D* sensor,
                                                                                                uint16_t interval,
                                                                                                uint8_t offset)
    : SensorNodeSensorDriver(0x54313248, 2, 0, interval, offset), sensor(sensor)
{
}

uint32_t SENSORNODE_SENSOR_OPTIMIZE SensorNodeSensorTemperatureHTU21D::readValue()
{
    return sensor->readRawTemperature();
}
