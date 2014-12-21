#pragma once

#include "global.h"


class __attribute__((packed,aligned(4))) DMXInChannel
{
public:
    virtual const uint8_t* getInDataPtr() = 0;
};

class __attribute__((packed,aligned(4))) DMXOutChannel
{
public:
    int lastSentTime;

    virtual bool isBusy() = 0;
    virtual uint8_t* getOutDataPtr() = 0;
    virtual void sendPacket() = 0;
};


extern DMXInChannel* dmxInChannel[DMX_IN_CHANNELS];
extern DMXOutChannel* dmxOutChannel[DMX_OUT_CHANNELS];


extern void initDMX();
