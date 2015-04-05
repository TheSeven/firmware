#pragma once
#include "global.h"
#include "protocol.h"


extern RadioPacket packet;


void handleReset();
void resetAnalyzer();
void initAnalyzer(const char* title);
void displayStats();
