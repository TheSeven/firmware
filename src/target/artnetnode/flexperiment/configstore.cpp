#include "global.h"
#include "target/artnetnode/flexperiment/configstore.h"
#include "target/artnetnode/configstore.h"
#include "sys/util.h"
#include "soc/stm32/spi.h"
#include "device/spiflash/spiflash.h"
#include "lib/configstore/configstore.h"
#include "lib/flashpartition/flashpartition.h"

#if defined(SOC_STM32F1)
#include "soc/stm32/f1/gpio.h"
#else
#include "soc/stm32/gpio.h"
#endif


#define PARTITION_ID 0x664374654e747241ull


union SystemConfig
{
    uint8_t d8[248];
    struct
    {
        struct
        {
            uint16_t size;
            uint16_t version;
            struct ArtNetNodeConfig data;
        } nodeCfg;
        struct
        {
            uint16_t size;
            uint16_t version;
            struct BoardConfig data;
        } boardCfg;
    } f;
};


static SPIFlash flash = SPIFlash(&STM32::SPI::SPI1, PIN_C2, 10000000, 100000000);
static FlashPartitionManager<15, 256> partMgr = FlashPartitionManager<15, 256>(&flash);
static StoragePartition configPart;
static ConfigData<SystemConfig, 256> config = ConfigData<SystemConfig, 256>();
static ConfigStore configStore = ConfigStore(&config);
struct ArtNetNodeConfig* const nodeCfg = &config.data.f.nodeCfg.data;
struct BoardConfig* const boardCfg = &config.data.f.boardCfg.data;
bool configChanged = false;


void upgradeBoardConfig(int oldVersion, int oldSize)
{
    switch (oldVersion)
    {
    }
}

void moveConfig(void* ptr, int size)
{
    uint8_t* base = (uint8_t*)ptr;
    int oldsize = *((uint16_t*)ptr);
    memmove(base + size, base + oldsize, sizeof(SystemConfig) - size);
    if (size > oldsize) memset(base + oldsize, 0, size - oldsize);
    else memset(base + sizeof(SystemConfig), 0, oldsize - size);
}

void initConfig()
{
    if (!partMgr.getPartition(&configPart, PARTITION_ID))
        partMgr.createPartition(PARTITION_ID, configStore.getSize(&flash));
    partMgr.getPartition(&configPart, PARTITION_ID);
    configStore.init(&configPart);
    configChanged = false;
    if (config.data.f.nodeCfg.size != sizeof(config.data.f.nodeCfg))
    {
        moveConfig(&config.data.f.nodeCfg, sizeof(config.data.f.nodeCfg));
        configChanged = true;
    }
    if (config.data.f.nodeCfg.version != ARTNETNODECONFIG_VERSION)
    {
        upgradeNodeConfig(config.data.f.nodeCfg.version, config.data.f.nodeCfg.size);
        configChanged = true;
    }
    if (config.data.f.boardCfg.version != BOARDCONFIG_VERSION)
    {
        upgradeBoardConfig(config.data.f.boardCfg.version, config.data.f.boardCfg.size);
        configChanged = true;
    }
    if (configChanged) saveConfig();
}

void saveConfig()
{
    configStore.save();
    configChanged = false;
}
