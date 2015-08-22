#include "global.h"
#include "fpnanobat.h"
#include "interface/clockgate/clockgate.h"
#include "soc/stm32/f0/clockgates.h"
#include "soc/stm32/f0/adc_regs.h"
#include "soc/stm32/gpio.h"


uint32_t SENSORNODE_SENSOR_OPTIMIZE SensorNodeSensorPowerSupplyFPNanoBat::readValue()
{
    return adc->readAux();
}
