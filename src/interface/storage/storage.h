#pragma once

#include "global.h"
#include "sys/util.h"


#ifndef STORAGEPARTITION_OPTIMIZE
#define STORAGEPARTITION_OPTIMIZE
#endif


class __attribute__((packed,aligned(4))) Storage
{
protected:
    constexpr Storage() : pageCount(0), pageSize(0), eraseSize(0), programSize(0) {}

    constexpr Storage(uint32_t pageCount, uint32_t pageSize, int eraseSize, int programSize)
        : pageCount(pageCount), pageSize(pageSize), eraseSize(eraseSize), programSize(programSize) {}

public:
    enum Result
    {
        RESULT_OK = 0,
        RESULT_UNKNOWN_ERROR,
        RESULT_INVALID_STATE,
        RESULT_INVALID_ARGUMENT,
        RESULT_RESOURCE_BUSY,
        RESULT_TIMEOUT,
        RESULT_COMM_ERROR,
        RESULT_DATA_ERROR,
        RESULT_UNSUPPORTED,
    };

    uint32_t pageCount;
    uint32_t pageSize;
    int eraseSize;
    int programSize;

    virtual enum Result reset() = 0;
    virtual enum Result getStatus() = 0;
    virtual enum Result read(uint32_t page, uint32_t len, void* buf) = 0;
    virtual enum Result write(uint32_t page, uint32_t len, const void* buf) = 0;
    virtual enum Result erase(uint32_t page, uint32_t len) = 0;
};

class __attribute__((packed,aligned(4))) StoragePartition final : public Storage
{
public:
    Storage* storage;
    uint32_t offset;

    constexpr STORAGEPARTITION_OPTIMIZE StoragePartition() : Storage(), storage(NULL), offset(0) {}

    constexpr STORAGEPARTITION_OPTIMIZE StoragePartition(Storage* storage, uint32_t offset, uint32_t size)
        : Storage(size * MAX(storage->programSize, storage->eraseSize), storage->pageSize,
                  storage->eraseSize, storage->programSize),
          storage(storage), offset(MAX(storage->programSize, storage->eraseSize) * offset) {}

    enum Result STORAGEPARTITION_OPTIMIZE reset()
    {
        return storage->reset();
    }

    enum Result STORAGEPARTITION_OPTIMIZE getStatus()
    {
        return storage->getStatus();
    }

    enum Result STORAGEPARTITION_OPTIMIZE read(uint32_t page, uint32_t len, void* buf)
    {
        if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
        return storage->read(page + offset, len, buf);
    }

    enum Result STORAGEPARTITION_OPTIMIZE write(uint32_t page, uint32_t len, const void* buf)
    {
        if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
        return storage->write(page + offset, len, buf);
    }

    enum Result STORAGEPARTITION_OPTIMIZE erase(uint32_t page, uint32_t len)
    {
        if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
        return storage->erase(page + offset, len);
    }
};
