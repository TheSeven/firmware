#pragma once
#include "global.h"
#include "device/sdhc_spi/sdhc_spi.h"
#include "device/pcd8544/pcd8544.h"
#include "lib/fatfs/fatfs.h"


extern SDHC_SPI sd;
extern FatFs::FileSystem fs;
extern PCD8544 lcd;
