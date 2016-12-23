#pragma once

#include "global.h"
#include "sys/util.h"


extern volatile bool wiggle;
extern volatile bool running;
extern volatile int8_t dir;
extern volatile int speed;
extern volatile int pos;
extern volatile int targetpos;


extern void setMicrostepping(uint32_t mode);
extern void initStepper();
