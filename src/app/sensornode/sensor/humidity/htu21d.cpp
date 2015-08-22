#include "global.h"
#include "app/sensornode/sensor/humidity/htu21d.h"


SENSORNODE_SENSOR_OPTIMIZE SensorNodeSensorHumidityHTU21D::SensorNodeSensorHumidityHTU21D(HTU21D* sensor,
                                                                                          uint16_t interval,
                                                                                          uint8_t offset)
    : SensorNodeSensorDriver(0x48313248, 2, 0, interval, offset), sensor(sensor)
{
}

uint32_t SENSORNODE_SENSOR_OPTIMIZE SensorNodeSensorHumidityHTU21D::readValue()
{
    return sensor->readRawHumidity();
}
