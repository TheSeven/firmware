#include "global.h"
#include "app/sensornode/storage.h"
#include "device/spiflash/spiflash.h"
#include "lib/flashpartition/flashpartition.h"


class SensorNodeStorageSPIFlash : public SensorNodeStorageDriver
{
    StoragePartition configPart;
    StoragePartition metaPart;
    StoragePartition dataPart;
    SPIFlash flash;

public:
    SensorNodeStorageSPIFlash() : SensorNodeStorageDriver(&configPart, &metaPart, &dataPart),
                                  flash(SENSORNODE_STORAGE_SPIFLASH_SPIBUS, SENSORNODE_STORAGE_SPIFLASH_CSPIN,
                                        SENSORNODE_STORAGE_SPIFLASH_SPIFREQ, SENSORNODE_STORAGE_SPIFLASH_SPIFREQ) {}
    void init();
    void stayAwake(bool on);
};


SensorNodeStorageSPIFlash flashStorage;
SensorNodeStorageDriver* storageDriver = &flashStorage;


void SENSORNODE_STORAGE_OPTIMIZE SensorNodeStorageSPIFlash::init()
{
    if (flash.pageSize != 1) hang();
    stayAwake(true);
    FlashPartitionManager<SENSORNODE_STORAGE_SPIFLASH_MAXPART, 1> partMgr(&flash);
    uint32_t blkSize = MAX(flash.programSize, flash.eraseSize);
    uint32_t cfgSize = configStore.getSize(&flash);
    if (!partMgr.getPartition(&configPart, SENSORNODE_STORAGE_SPIFLASH_PARTID))
    {
        partMgr.createMaxPartition(SENSORNODE_STORAGE_SPIFLASH_PARTID,
                                   cfgSize + 3, SENSORNODE_STORAGE_SPIFLASH_MAXSIZE);
        if (!partMgr.getPartition(&configPart, SENSORNODE_STORAGE_SPIFLASH_PARTID)) hang();
        configPart.erase(0, configPart.pageCount);
    }

    uint32_t offset = configPart.offset / blkSize;
    uint32_t space = configPart.pageCount / blkSize;
    new(&configPart) StoragePartition(&flash, offset + space - cfgSize, cfgSize);
    new(&metaPart) StoragePartition(&flash, offset, 2);
    new(&dataPart) StoragePartition(&flash, offset + 2, space - cfgSize - 2);
}

void SENSORNODE_STORAGE_OPTIMIZE SensorNodeStorageSPIFlash::stayAwake(bool on)
{
    flash.stayAwake(on);
}
