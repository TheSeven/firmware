#include "global.h"
#include "app/sensornode/sensor.h"
#include "app/sensornode/common.h"
#include "app/sensornode/storage.h"
#include "app/sensornode/sensor/powersupply/stm32vcc.h"
#include "app/sensornode/sensor/powersupply/fpnanobat.h"
#include "app/sensornode/sensor/temperature/ds1820.h"


SensorNodeSensorDriver* sensors[SENSORNODE_MAXSENSORS];
SensorNodeSensorPowerSupplySTM32VCC vccSensor;
SensorNodeSensorPowerSupplyFPNanoBat batSensor;
SensorNodeSensorTemperatureDS1820 tempSensor[SENSORNODE_MAXSENSORS - 2];
OneWire::Bus onewire(ONEWIRE_PIN);

static const char hextable[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};


void SENSORNODE_TARGET_OPTIMIZE sensornode_init_sensors()
{
    sensorCount = 0;

    new(&vccSensor) SensorNodeSensorPowerSupplySTM32VCC(config.data.boardConfig.sensorInterval[0],
                                                        config.data.boardConfig.sensorOffset[0]);
    memcpy(vccSensor.meta.attr.sensorName, "System voltage", 14);
    sensors[sensorCount++] = &vccSensor;
    new(&batSensor) SensorNodeSensorPowerSupplyFPNanoBat(&vccSensor, config.data.boardConfig.sensorInterval[1],
                                                         config.data.boardConfig.sensorOffset[1]);
    memcpy(batSensor.meta.attr.sensorName, "Battery voltage", 15);
    sensors[sensorCount++] = &batSensor;

    sensornode_enable_sensors();
    while (sensorCount < ARRAYLEN(sensors))
    {
        int i = sensorCount - 2;
        uint64_t* deviceId = onewire.discoverDevice();
        if (!deviceId) break;
        new(&tempSensor[i]) SensorNodeSensorTemperatureDS1820(&onewire, *deviceId,
                                                              config.data.boardConfig.sensorInterval[sensorCount],
                                                              config.data.boardConfig.sensorOffset[sensorCount]);
        char* ptr = tempSensor[i].meta.attr.sensorName;
        for (int d = 0; d < 16; d++)
        {
            *ptr++ = hextable[*deviceId & 0xf];
            *deviceId >>= 4;
        }
        sensors[sensorCount++] = &tempSensor[i];
    }
    sensornode_disable_sensors();
}

uint16_t SENSORNODE_TARGET_OPTIMIZE sensornode_get_persistent_interval(uint8_t sensorId)
{
    return config.data.boardConfig.sensorInterval[sensorId];
}

void SENSORNODE_TARGET_OPTIMIZE sensornode_set_persistent_sensor_config(uint8_t sensorId, SensorMeta* meta)
{
    config.data.boardConfig.sensorInterval[sensorId] = meta->attr.interval;
    config.data.boardConfig.sensorOffset[sensorId] = meta->offset;
}

bool SENSORNODE_TARGET_OPTIMIZE sensornode_flash_write_safe()
{
    //uint32_t voltage = psuSensor.readValue();
    //if ((voltage & 0xffff) < 1000) return false;
    //if ((voltage >> 16) < 1000) return false;
    return true;
}
