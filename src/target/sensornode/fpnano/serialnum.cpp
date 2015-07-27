#include "global.h"
#include "app/sensornode/common.h"
#include "app/sensornode/storage.h"


uint32_t SENSORNODE_TARGET_OPTIMIZE sensornode_get_hw_serial()
{
    return config.data.serialNumber;
}
