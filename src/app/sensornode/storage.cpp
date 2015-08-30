#include "global.h"
#include "storage.h"
#include "common.h"
#include "sensor.h"
#include "radio.h"
#include "sys/time.h"
#include "sys/util.h"


union __attribute__((packed,aligned(4))) DataHeader
{
    uint8_t u8[48];
    uint32_t u32[12];
    struct __attribute__((packed))
    {
        uint32_t measurementId;
        uint32_t dataOffset;
        uint8_t sensorId;
        uint8_t bytesPerPoint;
        int16_t pageSeq;
        uint64_t : 32;
        SensorAttributes attr;
        uint64_t : 63;
        bool invalid : 1;
    };
};

struct __attribute__((packed,aligned(4))) PaddedDataHeader
{
    DataHeader d;
    uint8_t padding[96 - sizeof(DataHeader)];
};

struct HistoryReadPtr
{
    uint32_t address;
    DataHeader d;
};


ConfigData<Config, 32> config;
ConfigStore configStore(&config);
uint32_t measurementId;
uint32_t measurementTime;
int16_t pageSeq;
uint32_t currentMeta;
uint32_t nextDataPage;
HistoryReadPtr histPtr;
uint32_t histLastMeta;


void SENSORNODE_STORAGE_OPTIMIZE eraseAllData()
{
    storageDriver->stayAwake(true);
    storageDriver->metaStorage->erase(0, storageDriver->metaStorage->pageCount);
    storageDriver->dataStorage->erase(0, storageDriver->dataStorage->pageCount);
    storageDriver->stayAwake(false);
}

void SENSORNODE_STORAGE_OPTIMIZE makeSpaceForSensorMeta()
{
    uint32_t eraseSize = storageDriver->dataStorage->eraseSize;
    uint32_t page = nextDataPage;
    for (int i = 0; i < sensorCount; i++)
    {
        if (!sensornode_flash_write_safe()) sensornode_powerdown();
        if (page >= storageDriver->dataStorage->pageCount) page = 0;
        if (histPtr.address == page) histPtr.d.invalid = true;
        storageDriver->dataStorage->erase(page, eraseSize);
        page += eraseSize;
    }
}


void SENSORNODE_STORAGE_OPTIMIZE writeDataPoint(uint8_t sensor, uint32_t data)
{
    SensorMeta* meta = &sensors[sensor]->meta;
    uint32_t eraseSize = storageDriver->dataStorage->eraseSize;
    uint32_t writePtr = sensors[sensor]->writePtr;
    uint32_t bytesPerPoint = sensors[sensor]->bytesPerPoint;
    if (!(writePtr & (eraseSize - 1)))
    {
        if (!sensornode_flash_write_safe()) sensornode_powerdown();
        if (nextDataPage >= storageDriver->dataStorage->pageCount) nextDataPage = 0;
        if (histPtr.address == nextDataPage) histPtr.d.invalid = true;
        storageDriver->dataStorage->erase(nextDataPage, eraseSize);
        DataHeader header;
        memset(&header, 0xff, sizeof(header));
        header.measurementId = measurementId;
        header.dataOffset = sensors[sensor]->dataOffset;
        header.sensorId = sensor;
        header.bytesPerPoint = bytesPerPoint;
        header.pageSeq = ++pageSeq;
        memcpy(&header.attr, &meta->attr, sizeof(header.attr));
        header.invalid = false;
        storageDriver->dataStorage->write(nextDataPage, sizeof(header), &header);
        writePtr = nextDataPage + sizeof(PaddedDataHeader);
        nextDataPage += eraseSize;
    }
    storageDriver->dataStorage->write(writePtr, bytesPerPoint, &data);
    sensors[sensor]->writePtr = writePtr + bytesPerPoint;
    sensors[sensor]->dataOffset += bytesPerPoint;
}


void SENSORNODE_STORAGE_OPTIMIZE writeMeta(uint32_t unixTime, uint16_t clientDevice, uint8_t flags)
{
    if (!sensornode_flash_write_safe()) sensornode_powerdown();
    MetaEntry meta;
    meta.measurementId = measurementId;
    meta.boardTime = now;
    meta.unixTime = unixTime;
    meta.clientDevice = clientDevice;
    meta.numSensors = sensorCount;
    meta.flags = flags;
    meta.invalid = false;
    currentMeta += sizeof(meta);
    if (!(currentMeta & (storageDriver->metaStorage->eraseSize - 1)))
    {
        if (currentMeta >= storageDriver->metaStorage->pageCount) currentMeta = 0;
        storageDriver->metaStorage->erase(currentMeta, storageDriver->metaStorage->eraseSize);
    }
    storageDriver->metaStorage->write(currentMeta, sizeof(meta), &meta);
}


void SENSORNODE_STORAGE_OPTIMIZE cacheHistoryDataBlock(int address)
{
    uint32_t size = storageDriver->dataStorage->pageCount;
    if (address >= (int)size) address -= size;
    else if (address < 0) address += size;
    if (!histPtr.d.invalid && (uint32_t)address == histPtr.address) return;
    histPtr.address = address;
    if (storageDriver->dataStorage->read(histPtr.address, sizeof(histPtr.d), &histPtr.d) != Storage::RESULT_OK) hang();
}


bool SENSORNODE_STORAGE_OPTIMIZE getHistorySensorAttributes(uint32_t measurementId, uint8_t sensorId,
                                                            SensorAttributes* ptr)
{
    uint32_t size = storageDriver->dataStorage->pageCount;
    uint32_t step = storageDriver->dataStorage->eraseSize;
    uint32_t base = histPtr.address;
    storageDriver->stayAwake(true);
    for (uint32_t o = 0; o < size; o += step)
    {
        cacheHistoryDataBlock(base + o);
        if (histPtr.d.invalid) continue;
        if (histPtr.d.measurementId != measurementId) continue;
        if (histPtr.d.sensorId != sensorId) continue;
        memcpy(ptr, &histPtr.d.attr, sizeof(*ptr));
        storageDriver->stayAwake(false);
        return true;
    }
    storageDriver->stayAwake(false);
    return false;
}

bool SENSORNODE_STORAGE_OPTIMIZE entryIsEmpty(uint8_t* addr, int len)
{
    while (len--) if (*--addr != 0xff) return false;
    return true;
}


bool SENSORNODE_STORAGE_OPTIMIZE getHistoryMeta(uint32_t* fromMeasId, uint32_t* fromTime, MetaEntry* ptr)
{
    uint32_t size = storageDriver->metaStorage->pageCount;
    uint32_t measurementId = 0xffffffff;
    uint32_t boardTime = 0xffffffff;
    uint32_t best = 0xffffffff;
    MetaEntry meta;
    bool foundOlder = false;
    bool foundMatching = false;
    storageDriver->stayAwake(true);
    for (uint32_t o = 0; o < size; o += sizeof(MetaEntry))
    {
        uint32_t addr = histLastMeta + o;
        if (addr >= size) addr -= size;
        if (storageDriver->metaStorage->read(addr, sizeof(MetaEntry), &meta) != Storage::RESULT_OK) hang();
        if (meta.invalid) continue;
        if (meta.measurementId < *fromMeasId || (meta.measurementId == *fromMeasId && meta.boardTime < *fromTime))
        {
            foundOlder = true;
            continue;
        }
        if (meta.measurementId > measurementId || (meta.measurementId == measurementId && meta.boardTime > boardTime))
            continue;
        foundMatching = true;
        best = addr;
        memcpy(ptr, &meta, sizeof(*ptr));
        measurementId = ptr->measurementId;
        boardTime = ptr->boardTime;
        if (foundOlder) break;
    }
    storageDriver->stayAwake(false);
    if (!foundMatching) return false;
    histLastMeta = best;
    *fromMeasId = measurementId;
    *fromTime = boardTime + 1;
    return true;
}


int SENSORNODE_STORAGE_OPTIMIZE getHistorySensorData(uint32_t measurementId, uint32_t* dataOffset, uint8_t sensorId,
                                                     void* ptr, uint32_t maxLen)
{
    uint32_t size = storageDriver->dataStorage->pageCount;
    uint32_t step = storageDriver->dataStorage->eraseSize;
    uint32_t dataSize = step - sizeof(PaddedDataHeader);
    uint32_t base = histPtr.address;
    uint32_t lastMeasId = 0;
    uint32_t addr = 0xffffffff;
    uint32_t oldest = 0xffffffff;
    storageDriver->stayAwake(true);
    for (uint32_t o = 0; addr == 0xffffffff && o < size; o += step)
    {
        cacheHistoryDataBlock(base + o);
        if (histPtr.d.invalid) break;
        if (histPtr.d.measurementId > measurementId) break;
        if (histPtr.d.measurementId < lastMeasId) break;
        lastMeasId = histPtr.d.measurementId;
        if (histPtr.d.measurementId != measurementId) continue;
        if (histPtr.d.sensorId != sensorId) continue;
        if (histPtr.d.dataOffset < oldest) oldest = histPtr.d.dataOffset;
        if (histPtr.d.dataOffset > *dataOffset) break;
        uint32_t offset = *dataOffset - histPtr.d.dataOffset;
        if (offset >= dataSize) continue;
        addr = histPtr.address + sizeof(PaddedDataHeader) + offset;
    }
    for (uint32_t o = 0; addr == 0xffffffff && o < size; o += step)
    {
        cacheHistoryDataBlock(base - o);
        if (histPtr.d.invalid) break;
        if (histPtr.d.measurementId < measurementId) break;
        if (histPtr.d.measurementId > lastMeasId) break;
        lastMeasId = histPtr.d.measurementId;
        if (histPtr.d.measurementId != measurementId) continue;
        if (histPtr.d.sensorId != sensorId) continue;
        if (histPtr.d.dataOffset < oldest) oldest = histPtr.d.dataOffset;
        if (histPtr.d.dataOffset > *dataOffset) continue;
        uint32_t offset = *dataOffset - histPtr.d.dataOffset;
        if (offset >= dataSize) break;
        addr = histPtr.address + sizeof(PaddedDataHeader) + offset;
    }
    storageDriver->stayAwake(false);
    if (addr == 0xffffffff) return oldest == 0xffffffff || *dataOffset > oldest ? 0 : *dataOffset- oldest;
    uint32_t len = MIN(maxLen, step - addr % step);
    if (storageDriver->dataStorage->read(addr, len, ptr) != Storage::RESULT_OK) hang();
    while (len && entryIsEmpty(((uint8_t*)ptr) + len, histPtr.d.bytesPerPoint)) len -= histPtr.d.bytesPerPoint;
    *dataOffset += len;
    return len;
}


void SENSORNODE_STORAGE_OPTIMIZE initStorage()
{
    storageDriver->init();
    storageDriver->stayAwake(true);
    if (configStore.init(storageDriver->configStorage) != Storage::RESULT_OK) hang();
    if (config.data.version > CONFIG_VERSION) memset(&config.data, 0, sizeof(config.data));
    if (config.data.version < 1)
    {
        config.data.maxCommTimeout = 60;
        config.data.idleBeaconInterval = 60;
        config.data.version = 1;
    }
    applyBeaconSettings(config.data.idleBeaconInterval, config.data.maxCommTimeout, config.data.localId);

    MetaEntry meta;
    measurementId = 0;
    uint32_t pageCount = storageDriver->metaStorage->pageCount;
    currentMeta = pageCount - sizeof(MetaEntry);
    uint32_t boardTime = 0;
    for (uint32_t offset = 0; offset < pageCount; offset += sizeof(MetaEntry))
    {
        if (storageDriver->metaStorage->read(offset, sizeof(MetaEntry), &meta) != Storage::RESULT_OK || meta.invalid)
        {
            if (meta.measurementId != 0xffffffff) currentMeta = offset;
            continue;
        }
        if (meta.measurementId > measurementId) measurementId = meta.measurementId;
        else if (meta.boardTime < boardTime) continue;
        boardTime = meta.boardTime;
        currentMeta = offset;
    }

    DataHeader header;
    nextDataPage = 0;
    uint32_t lastId = 0;
    pageSeq = 0;
    pageCount = storageDriver->dataStorage->pageCount;
    uint32_t eraseSize = storageDriver->dataStorage->eraseSize;
    for (nextDataPage = 0; nextDataPage < pageCount; nextDataPage += eraseSize)
    {
        if (storageDriver->dataStorage->read(nextDataPage, sizeof(DataHeader), &header) != Storage::RESULT_OK) break;
        if (header.invalid) break;
        if (header.measurementId > lastId) lastId = header.measurementId;
        else if (header.measurementId < lastId || header.pageSeq - pageSeq < 0) break;
        pageSeq = header.pageSeq;
    }
    histPtr.d.invalid = true;
    storageDriver->stayAwake(false);
    radioDriver->beacon->timeoutExpired();
}


void SENSORNODE_STORAGE_OPTIMIZE saveConfig()
{
    configStore.save();
}
