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


static void handleSectionUpgrade(void* ptr, int size, uint16_t version,
                                 void (*handler)(int oldVersion, int oldSize), bool last);
static void upgradeBoardConfig(int oldVersion, int oldSize);


template<typename T, int V, void (*U)(int oldVersion, int oldSize), bool L> class ConfigSection
{
public:
    uint16_t size;
    uint16_t version;
    T data;

    inline void handleUpgrades()
    {
        handleSectionUpgrade(this, sizeof(*this), V, U, L);
    }
};

union SystemConfig
{
    uint8_t d8[248];
    struct
    {
        ConfigSection<ArtNetNodeConfig, ARTNETNODECONFIG_VERSION, upgradeNodeConfig, false> nodeCfg;
        ConfigSection<BoardConfig, BOARDCONFIG_VERSION, upgradeBoardConfig, true> boardCfg;
    } f;
};


static SPIFlash flash(&STM32::SPI::SPI1, PIN_C2, 10000000, 100000000);
static FlashPartitionManager<15, 256> partMgr(&flash);
static StoragePartition configPart;
static ConfigData<SystemConfig, 256> config;
static ConfigStore configStore(&config);
struct ArtNetNodeConfig* const nodeCfg = &config.data.f.nodeCfg.data;
struct BoardConfig* const boardCfg = &config.data.f.boardCfg.data;
bool configChanged = false;


static void upgradeBoardConfig(int oldVersion, int oldSize)
{
    switch (oldVersion)
    {
    }
}

static void handleSectionUpgrade(void* ptr, int size, uint16_t version,
                                 void (*handler)(int oldVersion, int oldSize), bool last)
{
    uint8_t* base = (uint8_t*)ptr;
    ConfigSection<uint8_t, 0, (void(*)(int, int))0, true>* section = (typeof(section))ptr;
    int oldsize = section->size;
    if (section->size != size)
    {
        if (!last) memmove(base + size, base + section->size, sizeof(SystemConfig) - size);
        if (size > section->size) memset(base + section->size, 0, size - section->size);
        else memset(config.data.d8 + sizeof(SystemConfig) + size - section->size, 0, section->size - size);
        section->size = size;
        configChanged = true;
    }
    if (section->version != version)
    {
        if (section->version > version)
        {
            section->version = 0;
            memset(&section->data, 0, size - sizeof(*section) + sizeof(section->data));
        }
        handler(section->version, oldsize);
        section->version = version;
        configChanged = true;
    }
}

void initConfig()
{
    if (!partMgr.getPartition(&configPart, PARTITION_ID))
        partMgr.createPartition(PARTITION_ID, configStore.getSize(&flash));
    partMgr.getPartition(&configPart, PARTITION_ID);
    configStore.init(&configPart);
    configChanged = false;
    config.data.f.nodeCfg.handleUpgrades();
    config.data.f.boardCfg.handleUpgrades();
    if (configChanged) saveConfig();
}

void saveConfig()
{
    configStore.save();
    configChanged = false;
}
