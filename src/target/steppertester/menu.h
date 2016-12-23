#pragma once

#include "global.h"


struct Menu;

extern int showMenu(const Menu* menu);

struct __attribute__((packed)) MenuEntry
{
    const char* text;
    void (*renderer)(void* arg, int row);
    int (*handler)(void* arg, int row);
    void* arg;

    constexpr MenuEntry(const char* text, void (*renderer)(void* arg, int row), int (*handler)(void* arg, int row), void* arg)
        : text(text), renderer(renderer), handler(handler), arg(arg) {}
    constexpr MenuEntry(const char* text, int (*handler)(void* arg, int row), void* arg)
        : text(text), renderer(NULL), handler(handler), arg(arg) {}
    constexpr MenuEntry(const char* text, int (*handler)())
        : text(text), renderer(NULL), handler((int (*)(void*, int))handler), arg(NULL) {}
    constexpr MenuEntry(const char* text, const Menu* menu)
        : text(text), renderer(NULL), handler((int (*)(void*, int))showMenu), arg(const_cast<void*>((const void*)menu)) {}
    constexpr MenuEntry(const char* text)
        : text(text), renderer(NULL), handler(NULL), arg(NULL) {}
};

struct __attribute__((packed)) Menu
{
    uint8_t startRow;
    uint8_t height;
    uint8_t timeout;
    uint8_t entryCount;
    const char* title;
    MenuEntry entry[];

    constexpr Menu(uint8_t startRow, uint8_t height, uint8_t timeout, uint8_t entryCount, const char* title)
        : startRow(startRow), height(height), timeout(timeout), entryCount(entryCount), title(title), entry{} {}
};
