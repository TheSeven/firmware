#pragma once
#include "global.h"


#ifndef SENSORNODE_RTC_OPTIMIZE
#define SENSORNODE_RTC_OPTIMIZE
#endif


struct __attribute__((packed,aligned(4))) RTCState
{
    int time;
    int subsecond;
    int frequency;
};


class SensorNodeRTCDriver
{
public:
    virtual void reset() = 0;
    virtual int getTime() = 0;
    virtual void sleepUntil(int time) = 0;
    virtual void getState(RTCState* state) = 0;
};


extern SensorNodeRTCDriver* rtcDriver;
