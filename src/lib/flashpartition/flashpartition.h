#pragma once

#include "global.h"
#include "sys/util.h"
#include "lib/configstore/configstore.h"

#ifndef FLASHPARTITION_OPTIMIZE
#define FLASHPARTITION_OPTIMIZE
#endif


template<int M, int S> class __attribute__((packed,aligned(4))) FlashPartitionManager
{
private:
    struct PartitionDescriptor
    {
        uint64_t name;
        uint32_t offset;
        uint32_t size;
    };

    Storage* storage;
    StoragePartition superblock;
    ConfigData<PartitionDescriptor[M], S> partTable;
    ConfigStore store;

    PartitionDescriptor* FLASHPARTITION_OPTIMIZE findPartition(uint64_t name)
    {
        for (int i = 0; i < ARRAYLEN(partTable.data); i++)
            if (partTable.data[i].name == name)
                return &partTable.data[i];
        return NULL;
    }

public:
    FLASHPARTITION_OPTIMIZE FlashPartitionManager(Storage* storage) : storage(storage), store(&partTable)
    {
        uint32_t size = store.getSize(storage);
        uint32_t offset = storage->pageCount / MAX(storage->programSize, storage->eraseSize) - size;
        store.init(new(&superblock) StoragePartition(storage, offset, size));
    }

    bool FLASHPARTITION_OPTIMIZE getPartition(StoragePartition* partition, uint64_t name)
    {
        PartitionDescriptor* descriptor = findPartition(name);
        if (!descriptor) return false;
        new(partition) StoragePartition(storage, descriptor->offset, descriptor->size);
        return true;
    }

    Storage::Result FLASHPARTITION_OPTIMIZE deletePartition(uint64_t name)
    {
        PartitionDescriptor* descriptor = findPartition(name);
        if (!descriptor) return Storage::RESULT_INVALID_ARGUMENT;
        descriptor->name = 0;
        descriptor->offset = 0;
        descriptor->size = 0;
        return store.save();
    }

    Storage::Result FLASHPARTITION_OPTIMIZE createPartition(uint64_t name, uint32_t size)
    {
        PartitionDescriptor* descriptor = findPartition(name);
        if (descriptor) return Storage::RESULT_INVALID_ARGUMENT;
        descriptor = findPartition(0);
        if (!descriptor) return Storage::RESULT_UNKNOWN_ERROR;
        uint32_t end = superblock.offset / MAX(storage->programSize, storage->eraseSize);
        bool collision = true;
        while (collision)
        {
            if (end < size) return Storage::RESULT_UNKNOWN_ERROR;
            collision = false;
            for (int i = 0; i < ARRAYLEN(partTable.data); i++)
                if (partTable.data[i].offset < end && partTable.data[i].offset + partTable.data[i].size > end - size)
                {
                    end = partTable.data[i].offset;
                    collision = true;
                    break;
                }
        }
        descriptor->name = name;
        descriptor->offset = end - size;
        descriptor->size = size;
        return store.save();
    }

};
