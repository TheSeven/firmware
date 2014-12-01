#include "global.h"
#include "diskio.h"
#include "ffconf.h"
#include "sys/util.h"
#include "interface/storage/storage.h"


Storage* ff_device[_FF_PHYS_DRIVES];


DRESULT translate_result(Storage::Result result)
{
    if (result == Storage::RESULT_OK) return RES_OK;
    if (result == Storage::RESULT_INVALID_STATE || result == Storage::RESULT_RESOURCE_BUSY) return RES_NOTRDY;
    if (result == Storage::RESULT_INVALID_ARGUMENT || result == Storage::RESULT_UNSUPPORTED) return RES_PARERR;
    return RES_ERROR;
}

DSTATUS disk_status(BYTE pdrv)
{
    return ff_device[pdrv]->getStatus() == Storage::RESULT_OK ? 0 : STA_NOINIT;
}

DSTATUS disk_initialize(BYTE pdrv)
{
    return disk_status(pdrv);
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    int pagefactor = MAX(ff_device[pdrv]->programSize, ff_device[pdrv]->eraseSize);
    return translate_result(ff_device[pdrv]->read(sector * pagefactor, count * pagefactor, buff));
}

#if _USE_WRITE
DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    int pagefactor = MAX(ff_device[pdrv]->programSize, ff_device[pdrv]->eraseSize);
    Storage::Result result = Storage::RESULT_OK;
    if (ff_device[pdrv]->eraseSize) result = ff_device[pdrv]->erase(sector * pagefactor, count * pagefactor);
    if (result == Storage::RESULT_OK) result = ff_device[pdrv]->write(sector * pagefactor, count * pagefactor, buff);
    return translate_result(result);
}
#endif

#if _USE_IOCTL
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    switch (cmd)
    {
    case CTRL_SYNC: return RES_OK;
    case CTRL_TRIM:
    {
        uint32_t* data = (uint32_t*)buff;
        int pagefactor = MAX(ff_device[pdrv]->programSize, ff_device[pdrv]->eraseSize);
        Storage::Result result = ff_device[pdrv]->erase(data[0] * pagefactor, (data[1] - data[0] + 1) * pagefactor);
        return translate_result(result);
    }
    case GET_BLOCK_SIZE:
    case GET_SECTOR_SIZE:
    {
        uint16_t* data = (uint16_t*)buff;
        *data = ff_device[pdrv]->pageSize * MAX(ff_device[pdrv]->programSize, ff_device[pdrv]->eraseSize);
        return RES_OK;
    }
    case GET_SECTOR_COUNT:
    {
        uint16_t* data = (uint16_t*)buff;
        *data = ff_device[pdrv]->pageCount / MAX(ff_device[pdrv]->programSize, ff_device[pdrv]->eraseSize);
        return RES_OK;
    }
    default: return RES_PARERR;
    }
}
#endif
