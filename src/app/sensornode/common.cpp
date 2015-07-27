#include "global.h"
#include "common.h"

uint8_t sensorCount;
LiveData liveData;
uint8_t liveDataSize;
int now;
bool deepSleep;

__attribute__((weak)) void sensornode_sysinit()
{
}
