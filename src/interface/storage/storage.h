#pragma once

#include "global.h"


class __attribute__((packed,aligned(4))) Storage
{
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
