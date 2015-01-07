#pragma once

#include "global.h"

#define ARTNETNODECONFIG_VERSION 0

struct ArtNetNodeConfig
{

};


extern struct ArtNetNodeConfig* const nodeCfg;
extern bool configChanged;

extern void initConfig();
extern void saveConfig();
extern void upgradeNodeConfig(int oldVersion, int oldSize);
