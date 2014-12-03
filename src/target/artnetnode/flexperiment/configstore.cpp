#include "global.h"
#include "target/artnetnode/configstore.h"
#include "sys/util.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/spi.h"
#include "device/spiflash/spiflash.h"
#include "lib/configstore/configstore.h"
#include "lib/flashpartition/flashpartition.h"


#define PARTITION_ID 0x664374654e747241ull


union SystemConfig
{
    uint8_t d8[248];
    struct
    {
        struct ArtNetNodeConfig nodeCfg;
    } f;
};


static SPIFlash flash = SPIFlash(&STM32::SPI::SPI1, PIN_C2, 10000000, 100000000);
static FlashPartitionManager<15, 256> partMgr = FlashPartitionManager<15, 256>(&flash);
static StoragePartition configPart;
static ConfigData<SystemConfig, 256> config = ConfigData<SystemConfig, 256>();
static ConfigStore configStore = ConfigStore(&config);
struct ArtNetNodeConfig* const nodeCfg = &config.data.f.nodeCfg;


void initConfig()
{
    if (!partMgr.getPartition(&configPart, PARTITION_ID))
        partMgr.createPartition(PARTITION_ID, configStore.getSize(&flash));
    partMgr.getPartition(&configPart, PARTITION_ID);
    configStore.init(&configPart);
}

void saveConfig()
{
    configStore.save();
}
