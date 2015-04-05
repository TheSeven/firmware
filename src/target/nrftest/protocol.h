#pragma once
#include "global.h"


union RadioPacket
{
    uint8_t byte[32];
    struct Fields
    {
        uint32_t sent;
        uint32_t retrans;
        uint32_t received;
        uint32_t acked;
        uint32_t success;
        bool reset;
    } f;
};
