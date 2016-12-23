#pragma once

#include "global.h"
#include "device/pcd8544/pcd8544.h"


extern PCD8544 lcd;


extern void initLCD();
extern void drawHeader();
extern void drawStatus();
