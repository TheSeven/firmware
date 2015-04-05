#pragma once
#include "global.h"
#include "device/nrf/nrf_radio.h"


extern NRF::Radio* radio;


extern void initRadio();
extern void pollRadio();
