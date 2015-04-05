#pragma once
#include "global.h"
#include "app/main.h"


void transmittedHandler(bool success, int retransmissions);
void receivedHandler(int pipe, uint8_t* packet, int length);
