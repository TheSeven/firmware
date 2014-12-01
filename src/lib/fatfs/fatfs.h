#pragma once

#include "global.h"
#include "interface/storage/storage.h"
#include "ff.h"


namespace FatFs
{

    class __attribute__((packed,aligned(4))) FileSystem
    {
    public:
        FileSystem(Storage* storage, int partition, const char* volName, FRESULT* result);
        ~FileSystem();
        FRESULT getFree(uint32_t* clusters);

    private:
        FATFS fs;
        const char* volName;
    };

    struct __attribute__((packed)) FileMode
    {
        bool read : 1;
        bool write : 1;
        bool createNew : 1;
        bool createAlways : 1;
        bool _written : 1;
        bool _dirty : 1;
        uint8_t : 1;
    };

    class __attribute__((packed,aligned(4))) File
    {
    public:
        File(const char* path, FileMode mode, FRESULT* result);
        constexpr File() : file({ 0 }) {}
        ~File();
        FRESULT close();
        FRESULT read(void* buf, uint32_t len, uint32_t* read);
        FRESULT write(const void* buf, uint32_t len, uint32_t* written);
        FRESULT forward(uint32_t (*handler)(const uint8_t* buf, uint32_t len), uint32_t len, uint32_t* forwarded);
        FRESULT lseek(uint32_t offset);
        FRESULT truncate();
        FRESULT sync();
        int putc(char c);
        int puts(const char* str);
        int printf(const char* format, ...);
        char* gets(char* buf, int len);
        bool eof();
        uint8_t error();
        uint32_t tell();
        uint32_t size();

    private:
        struct PrintfState
        {
            File* obj;
            int bytes;
            bool error;
        };

        FIL file;

        static int printfChar(void* state, uint8_t data);
    };

    class __attribute__((packed,aligned(4))) Directory
    {
    public:
        Directory(const char* path, FRESULT* result);
        constexpr Directory() : directory({ 0 }) {}
        ~Directory();
        FRESULT close();
        FRESULT readNext(FILINFO* fileInfo);

    private:
        DIR directory;
    };

    FRESULT mkdir(const char* path);
    FRESULT unlink(const char* path);
    FRESULT rename(const char* oldPath, const char* newPath);
    FRESULT stat(const char* path, FILINFO* fileInfo);
    FRESULT chmod(const char* path, uint8_t value, uint8_t mask);
    FRESULT utime(const char* path, const FILINFO* fileInfo);
    FRESULT chdir(const char* path);
    FRESULT chdrive(const char* path);
    FRESULT getcwd(char* buf, uint32_t len);
    FRESULT getlabel(const char* path, char* label, uint32_t* serial);
    FRESULT setlabel(const char* label);
    FRESULT mkfs(const char* path, uint8_t superFloppy, uint32_t allocationUnit);
    FRESULT fdisk(Storage* storage, const uint32_t partitionSizes[], void* sectorBuffer);

}
