#include "global.h"


#ifndef SENSORNODE_RTC_OPTIMIZE
#define SENSORNODE_RTC_OPTIMIZE
#endif


class SensorNodeRTCDriver
{
public:
    virtual void reset() = 0;
    virtual int getTime() = 0;
    virtual void sleepUntil(int time) = 0;
};


extern SensorNodeRTCDriver* rtcDriver;
