#pragma once

#include "global.h"
#include "sys/util.h"

template<typename T> class __attribute__((packed,aligned(4))) RingBuffer
{
private:
    volatile uint16_t headPos;
    volatile uint16_t tailPos;
    T buf;

public:
    constexpr RingBuffer() : headPos(0), tailPos(0) {}

    bool put(const T* data)
    {
        int head = headPos;
        int free = tailPos - head - 1;
        if (free < 0) free += ARRAYLEN(buf);
        if (!free) return false;
        memcpy(buf + head++, data, sizeof(*buf));
        if (head == ARRAYLEN(buf)) head = 0;
        headPos = head;
        return true;
    }

    size_t put(const T* data, size_t n)
    {
        int head = headPos;
        int free = tailPos - head - 1;
        if (free < 0) free += ARRAYLEN(buf);
        n = MIN(n, free);
        int size = MIN(n, ARRAYLEN(buf) - head);
        if (size)
        {
            memcpy(buf + head, data, size * sizeof(*buf));
            data += size;
            head += size;
        }
        size = n - size;
        if (head == ARRAYLEN(buf)) head = 0;
        if (size)
        {
            memcpy(buf, data, size * sizeof(*buf));
            head += size;
        }
        headPos = head;
        return n;
    }

    bool get(T* data)
    {
        int tail = tailPos;
        int used = headPos - tail;
        if (used < 0) used += ARRAYLEN(buf);
        if (!used) return false;
        memcpy(buf + tail++, data, sizeof(*buf));
        if (tail == ARRAYLEN(buf)) tail = 0;
        tailPos = tail;
        return true;
    }

    size_t get(T* data, size_t n)
    {
        int tail = tailPos;
        int used = headPos - tail;
        if (used < 0) used += ARRAYLEN(buf);
        n = MIN(n, used);
        int size = MIN(n, ARRAYLEN(buf) - tail);
        if (size)
        {
            memcpy(data, buf + tail, size * sizeof(*buf));
            data += size;
            tail += size;
        }
        size = n - size;
        if (tail == ARRAYLEN(buf)) tail = 0;
        if (size)
        {
            memcpy(data, buf, size * sizeof(*buf));
            tail += size;
        }
        tailPos = tail;
        return n;
    }

    size_t used()
    {
        int used = headPos - tailPos;
        if (used < 0) used += ARRAYLEN(buf);
        return used;
    }

    size_t free()
    {
        int free = tailPos - headPos - 1;
        if (free < 0) free += ARRAYLEN(buf);
        return free;
    }

};
