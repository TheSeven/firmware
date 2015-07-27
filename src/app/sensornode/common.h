#pragma once
#include "global.h"


#ifndef SENSORNODE_CORE_OPTIMIZE
#define SENSORNODE_CORE_OPTIMIZE
#endif
#ifndef SENSORNODE_TARGET_OPTIMIZE
#define SENSORNODE_TARGET_OPTIMIZE
#endif


struct __attribute__((packed)) LiveData
{
    uint8_t measId;
    uint32_t : 8;
    uint16_t boardTime;
    uint8_t sensorData[24];
};


extern uint8_t sensorCount;
extern LiveData liveData;
extern uint8_t liveDataSize;
extern int now;
extern bool deepSleep;


extern void updateLiveDataSize();


extern uint32_t sensornode_get_hw_serial();
extern void sensornode_sysinit();
extern void sensornode_enable_sensors();
extern void sensornode_disable_sensors();
extern bool sensornode_flash_write_safe();
extern void sensornode_powerdown();
extern void sensornode_reboot();
