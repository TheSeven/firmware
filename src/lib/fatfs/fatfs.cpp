#include "global.h"
#include "lib/fatfs/fatfs.h"
#include "lib/printf/printf.h"
#include "sys/util.h"
#include "sys/stdarg.h"


extern Storage* ff_device[_FF_PHYS_DRIVES];


FatFs::FileSystem::FileSystem(Storage* storage, int partition, const char* volName, FRESULT* result) : volName(volName)
{
    memset(&fs, 0, sizeof(fs));
    for (fs.drv = 0; fs.drv < _FF_PHYS_DRIVES; fs.drv++)
        if (!ff_device[fs.drv]) break;
    if (fs.drv >= _FF_PHYS_DRIVES) return;
    ff_device[fs.drv] = storage;
    fs.part = partition;
    FRESULT res = f_mount(&fs, volName, true);
    if (result) *result = res;
}

FatFs::FileSystem::~FileSystem()
{
    if (fs.drv >= _FF_PHYS_DRIVES) return;
    f_mount(NULL, volName, false);
    ff_device[fs.drv] = NULL;
}

FRESULT FatFs::FileSystem::getFree(uint32_t* clusters)
{
    FATFS* fsPtr;
    return f_getfree(volName, clusters, &fsPtr);
}

FatFs::File::File(const char* path, FileMode mode, FRESULT* result)
{
    FRESULT res = f_open(&file, path, *((uint8_t*)&mode));
    if (result) *result = res;
}

FatFs::File::~File()
{
    close();
}

FRESULT FatFs::File::close()
{
    return f_close(&file);
}

FRESULT FatFs::File::read(void* buf, uint32_t len, uint32_t* read)
{
    return f_read(&file, buf, len, read);
}

FRESULT FatFs::File::write(const void* buf, uint32_t len, uint32_t* written)
{
    return f_write(&file, buf, len, written);
}

FRESULT FatFs::File::forward(uint32_t (*handler)(const uint8_t* buf, uint32_t len), uint32_t len, uint32_t* forwarded)
{
    return f_forward(&file, handler, len, forwarded);
}

FRESULT FatFs::File::lseek(uint32_t offset)
{
    return f_lseek(&file, offset);
}

FRESULT FatFs::File::truncate()
{
    return f_truncate(&file);
}

FRESULT FatFs::File::sync()
{
    return f_sync(&file);
}

int FatFs::File::putc(char c)
{
    return f_putc(c, &file);
}

int FatFs::File::puts(const char* str)
{
    return f_puts(str, &file);
}

int FatFs::File::printfChar(void* state, uint8_t data)
{
    struct PrintfState* printfState = (struct PrintfState*)state;
    int result = printfState->obj->putc(data);
    if (result <= 0) printfState->error = true;
    if (result > 0) printfState->bytes += result;
    return !printfState->error;
}

int FatFs::File::printf(const char* format, ...)
{
    if (!format || !*format) return 0;
    struct PrintfState printfState;
    printfState.obj = this;
    printfState.bytes = 0;
    printfState.error = false;
    va_list args;
    va_start(args, format);
    printf_format(&printfChar, &printfState, format, args);
    va_end(args);
    return printfState.error ? -1 : printfState.bytes;
}

char* FatFs::File::gets(char* buf, int len)
{
    return f_gets(buf, len, &file);
}

bool FatFs::File::eof()
{
    return f_eof(&file);
}

uint8_t FatFs::File::error()
{
    return f_error(&file);
}

uint32_t FatFs::File::tell()
{
    return f_tell(&file);
}

uint32_t FatFs::File::size()
{
    return f_size(&file);
}

FatFs::Directory::Directory(const char* path, FRESULT* result)
{
    FRESULT res = f_opendir(&directory, path);
    if (result) *result = res;
}

FatFs::Directory::~Directory()
{
    close();
}

FRESULT FatFs::Directory::close()
{
    return f_closedir(&directory);
}

FRESULT FatFs::Directory::readNext(FILINFO* fileInfo)
{
    return f_readdir(&directory, fileInfo);
}

FRESULT FatFs::mkdir(const char* path)
{
    return f_mkdir(path);
}

FRESULT FatFs::unlink(const char* path)
{
    return f_unlink(path);
}

FRESULT FatFs::rename(const char* oldPath, const char* newPath)
{
    return f_rename(oldPath, newPath);
}

FRESULT FatFs::stat(const char* path, FILINFO* fileInfo)
{
    return f_stat(path, fileInfo);
}

FRESULT FatFs::chmod(const char* path, uint8_t value, uint8_t mask)
{
    return f_chmod(path, value, mask);
}

FRESULT FatFs::utime(const char* path, const FILINFO* fileInfo)
{
    return f_utime(path, fileInfo);
}

FRESULT FatFs::chdir(const char* path)
{
    return f_chdir(path);
}

FRESULT FatFs::chdrive(const char* path)
{
    return f_chdrive(path);
}

FRESULT FatFs::getcwd(char* buf, uint32_t len)
{
    return f_getcwd(buf, len);
}

FRESULT FatFs::getlabel(const char* path, char* label, uint32_t* serial)
{
    return f_getlabel(path, label, serial);
}

FRESULT FatFs::setlabel(const char* label)
{
    return f_setlabel(label);
}

FRESULT FatFs::mkfs(const char* path, uint8_t superFloppy, uint32_t allocationUnit)
{
    return f_mkfs(path, superFloppy, allocationUnit);
}

FRESULT FatFs::fdisk(Storage* storage, const uint32_t partitionSizes[], void* sectorBuffer)
{
    int drive;
    for (drive = 0; drive < _FF_PHYS_DRIVES; drive++)
        if (ff_device[drive] == storage) return f_fdisk(drive, partitionSizes, sectorBuffer);
    for (drive = 0; drive < _FF_PHYS_DRIVES; drive++)
        if (!ff_device[drive]) break;
    if (drive >= _FF_PHYS_DRIVES) return FR_TOO_MANY_OPEN_FILES;
    ff_device[drive] = storage;
    FRESULT result = f_fdisk(drive, partitionSizes, sectorBuffer);
    ff_device[drive] = NULL;
    return result;
}
