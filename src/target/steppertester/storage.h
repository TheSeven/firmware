#pragma once

#include "global.h"
#include "lib/configstore/configstore.h"


#define CONFIG_VERSION 1
struct __attribute__((packed,aligned(4))) Config
{
    uint8_t version;
    volatile uint8_t driver;
    volatile uint8_t microstepping;
    uint8_t : 8;
    volatile int maxSpeed;
    volatile int accelFwd;
    volatile int brakeFwd;
    volatile int accelBwd;
    volatile int brakeBwd;
    volatile int wiggleTo;
};

extern ConfigData<Config, 256> cfg;
extern bool configChanged;

extern bool initStorage();
extern void saveConfig();
