#include "global.h"
#include "app/main.h"
#include "sys/util.h"
#include "lib/configstore/configstore.h"
#include "lib/crc32/crc32.h"


Storage::Result ConfigStore::init(Storage* storage)
{
    uint32_t partSize = getSize(storage) * MAX(storage->programSize, storage->eraseSize);
    if (storage->pageCount < partSize) return Storage::RESULT_INVALID_ARGUMENT;
    this->storage = storage;
    int dataSize = data->size;
    int blockSize = data->size + sizeof(ConfigDataHeader);
    int pages = blockSize / storage->pageSize;
    uint32_t bestAddr = partSize;
    int16_t bestUsn;
    bool valid = false;
    bool haveEmpty = false;
    Storage::Result result;
    for (uint32_t addr = 0; addr < partSize; addr += pages)
    {
        if ((result = storage->read(addr, pages, data)) != Storage::RESULT_OK) continue;
        if (!haveEmpty)
        {
            bool empty = true;
            for (int i = 0; i < blockSize / 4; i++)
                if (((int32_t*)data)[i] != -1)
                {
                    empty = false;
                    break;
                }
            if (empty)
            {
                haveEmpty = true;
                nextFree = addr;
            }
        }
        if (data->size > blockSize || (data->size & 3)
         || crc32(&data->size, data->size + sizeof(ConfigDataHeader) - sizeof(data->crc)) != data->crc)
        {
            result = Storage::RESULT_DATA_ERROR;
            continue;
        }
        if (!valid || data->usn - bestUsn > 0)
        {
            bestUsn = data->usn;
            bestAddr = addr;
            valid = true;
        }
    }
    if (valid && (result = storage->read(bestAddr, pages, data)) != Storage::RESULT_OK) valid = false;
    if (valid && crc32(&data->size, data->size + sizeof(ConfigDataHeader) - sizeof(data->crc)) != data->crc)
    {
        result = Storage::RESULT_DATA_ERROR;
        valid = false;
    }
    if (!valid) memset(data, 0, blockSize);
    data->size = dataSize;
    if (!haveEmpty)
    {
        uint32_t boundary = partSize >> 1;
        if (bestAddr < boundary) nextFree = boundary;
        else nextFree = 0;
        if ((result = storage->erase(nextFree, boundary)) != Storage::RESULT_OK) return result;
    }
    return Storage::RESULT_OK;
}

uint32_t ConfigStore::getSize(Storage* storage)
{
    int blockSize = storage->pageSize * MAX(storage->programSize, storage->eraseSize);
    int dataSize = data->size + sizeof(ConfigDataHeader);
    return (dataSize + blockSize - 1) / blockSize * 2;
}

Storage::Result ConfigStore::save()
{
    if (crc32(data, data->size + sizeof(ConfigDataHeader) - sizeof(data->crc)) == data->crc)
        return Storage::RESULT_OK;
    data->usn++;
    data->crc = crc32(&data->size, data->size + sizeof(ConfigDataHeader) - sizeof(data->crc));
    int blockSize = data->size + sizeof(ConfigDataHeader);
    int pages = blockSize / storage->pageSize;
    Storage::Result result;
    if ((result = storage->write(nextFree, pages, data)) != Storage::RESULT_OK) return result;
    nextFree += pages;
    uint32_t partSize = getSize(storage) * MAX(storage->programSize, storage->eraseSize);
    if (!(nextFree & ((partSize >> 1) - 1)))
    {
        if (nextFree >= partSize) nextFree = 0;
        if ((result = storage->erase(nextFree, partSize >> 1)) != Storage::RESULT_OK) return result;
    }
    return Storage::RESULT_OK;
}
