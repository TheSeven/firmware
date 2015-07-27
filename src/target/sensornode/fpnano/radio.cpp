#include "global.h"
#include "app/sensornode/common.h"
#include "app/sensornode/radio.h"


RadioPacket::StatusCode SENSORNODE_TARGET_OPTIMIZE sensornode_handle_custom_command(RadioPacket* packet, int length)
{
    return RadioPacket::StatusUnknownCommand;
}

void SENSORNODE_TARGET_OPTIMIZE sensornode_factory_programming(uint8_t* args, int length)
{
}
