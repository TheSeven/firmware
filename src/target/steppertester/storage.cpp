#include "global.h"
#include "storage.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/flash.h"
#include "lib/configstore/configstore.h"
#include "sys/util.h"


static const Config defaultConfig =
{
    version: CONFIG_VERSION,
    driver: 0,
    microstepping: 0,
    maxSpeed: 160000,
    accelFwd: 200,
    brakeFwd: 500,
    accelBwd: 200,
    brakeBwd: 500,
    wiggleTo: 0,
};

static StoragePartition configPart(&STM32::Flash, STM32::Flash.pageCount / STM32::Flash.eraseSize - 2, 2);
ConfigData<Config, 256> cfg;
static ConfigStore configStore(&cfg);
bool configChanged = false;


bool initStorage()
{
    bool ok = true;
    Storage::Result result;
    result = configStore.init(&configPart);
    if (result != Storage::RESULT_OK) hang();
    configChanged = false;
    if (cfg.data.version != CONFIG_VERSION)
    {
        ok = false;
        configChanged = true;
        memcpy(&cfg.data, &defaultConfig, sizeof(cfg.data));
        if (sizeof(cfg.padding)) memset(cfg.padding, 0, sizeof(cfg.padding));
    }
    return ok;
}

void saveConfig()
{
    Storage::Result result = configStore.save();
    if (result != Storage::RESULT_OK) hang();
    configChanged = false;
}
