#pragma once

#include "global.h"


extern uint32_t getButtonState();
extern void initButton();
extern uint32_t waitButton(uint32_t timeout);
extern uint32_t waitButtonRepeat(uint32_t interval);
