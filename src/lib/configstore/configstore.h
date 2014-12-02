#pragma once

#include "global.h"
#include "sys/util.h"
#include "interface/storage/storage.h"

class __attribute__((packed,aligned(4))) ConfigDataHeader
{
    public:
        uint32_t crc;
        uint16_t size;
        int16_t usn;

        constexpr ConfigDataHeader(uint16_t size) : crc(0), size(size), usn(0) {}
};

template<typename T, int S> class __attribute__((packed,aligned(4))) ConfigData : public ConfigDataHeader
{
public:
    constexpr ConfigData() : ConfigDataHeader(((1ull << 32) >> __builtin_clz(sizeof(T) + 7)) - 8) {}
    T data;
    uint8_t padding[(sizeof(T) + S + 7) / S * S - sizeof(T) - 8];
};

class __attribute__((packed,aligned(4))) ConfigStore
{
private:
    Storage* storage;
    ConfigDataHeader* data;
    uint32_t nextFree;

public:
    constexpr ConfigStore(ConfigDataHeader* data) : storage(NULL), data(data), nextFree(0) {}
    Storage::Result init(Storage* storage);
    uint32_t getSize(Storage* storage);
    Storage::Result save();
};
