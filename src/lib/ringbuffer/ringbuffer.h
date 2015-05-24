#pragma once

#include "global.h"
#include "sys/util.h"

template<typename T, size_t C> class __attribute__((packed,aligned(4))) RingBuffer
{
private:
    volatile uint16_t headPos;
    volatile uint16_t tailPos;
    T buf[C];

public:
    constexpr RingBuffer() : headPos(0), tailPos(0) {}

    bool poke(const T* data)
    {
        int head = headPos;
        int free = tailPos - head - 1;
        if (free < 0) free += C;
        if (!free) return false;
        memcpy(buf + head, data, sizeof(T));
        return true;
    }

    bool poke(const T** ptr)
    {
        int head = headPos;
        int free = tailPos - head - 1;
        if (free < 0) free += C;
        if (!free) return false;
        *ptr = buf + head;
        return true;
    }

    bool put(const T* data)
    {
        int head = headPos;
        int free = tailPos - head - 1;
        if (free < 0) free += C;
        if (!free) return false;
        memcpy(buf + head++, data, sizeof(T));
        if (head == C) head = 0;
        headPos = head;
        return true;
    }

    bool put(const T** ptr)
    {
        int head = headPos;
        int free = tailPos - head - 1;
        if (free < 0) free += C;
        if (!free) return false;
        *ptr = buf + head++;
        if (head == C) head = 0;
        headPos = head;
        return true;
    }

    size_t put(const T* data, size_t n)
    {
        int head = headPos;
        int free = tailPos - head - 1;
        if (free < 0) free += C;
        n = MIN(n, free);
        int size = MIN(n, C - head);
        if (size)
        {
            memcpy(buf + head, data, size * sizeof(T));
            data += size;
            head += size;
        }
        size = n - size;
        if (head == C) head = 0;
        if (size)
        {
            memcpy(buf, data, size * sizeof(T));
            head += size;
        }
        headPos = head;
        return n;
    }

    bool peek(T* data)
    {
        int tail = tailPos;
        int used = headPos - tail;
        if (used < 0) used += C;
        if (!used) return false;
        memcpy(buf + tail, data, sizeof(T));
        return true;
    }

    bool peek(const T** ptr)
    {
        int tail = tailPos;
        int used = headPos - tail;
        if (used < 0) used += C;
        if (!used) return false;
        *ptr = buf + tail;
        return true;
    }

    bool get(T* data)
    {
        int tail = tailPos;
        int used = headPos - tail;
        if (used < 0) used += C;
        if (!used) return false;
        memcpy(data, buf + tail++, sizeof(T));
        if (tail == C) tail = 0;
        tailPos = tail;
        return true;
    }

    bool get(const T** ptr)
    {
        int tail = tailPos;
        int used = headPos - tail;
        if (used < 0) used += C;
        if (!used) return false;
        *ptr = buf + tail++;
        if (tail == C) tail = 0;
        tailPos = tail;
        return true;
    }

    size_t get(T* data, size_t n)
    {
        int tail = tailPos;
        int used = headPos - tail;
        if (used < 0) used += C;
        n = MIN(n, used);
        int size = MIN(n, C - tail);
        if (size)
        {
            memcpy(data, buf + tail, size * sizeof(T));
            data += size;
            tail += size;
        }
        size = n - size;
        if (tail == C) tail = 0;
        if (size)
        {
            memcpy(data, buf, size * sizeof(T));
            tail += size;
        }
        tailPos = tail;
        return n;
    }

    size_t used()
    {
        int used = headPos - tailPos;
        if (used < 0) used += C;
        return used;
    }

    size_t free()
    {
        int free = tailPos - headPos - 1;
        if (free < 0) free += C;
        return free;
    }

};
