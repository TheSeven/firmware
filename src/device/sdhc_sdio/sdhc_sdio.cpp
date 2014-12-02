#include "global.h"
#include "device/sdhc_sdio/sdhc_sdio.h"
#include "sys/time.h"
#include "sys/util.h"


SDHC_SDIO::SDHC_SDIO(SDIO* sdio, int maxSpeed, int retries)
    : sdio(sdio), maxSpeed(maxSpeed), retries(retries)
{
    pageSize = 512;
    programSize = 1;
    eraseSize = 0;
    reset();
}

enum Storage::Result SDHC_SDIO::checkResponse(uint32_t response)
{
    if (response & 0xff41a008) return RESULT_INVALID_ARGUMENT;
    if (response & 0x00800000) return RESULT_COMM_ERROR;
    if (response & 0x00200000) return RESULT_DATA_ERROR;
    if (response & 0x001e00d7) return RESULT_UNKNOWN_ERROR;
    return RESULT_OK;
}

enum Storage::Result SDHC_SDIO::prepareACMD()
{
    enum Storage::Result rc;
    uint32_t resp;
    // Send CMD55 to tell the card that the next command is an application command
    if ((rc = sdio->command({ .cmd = 55, .responseType = SDIO::RESPONSE_SHORT }, rca << 16, &resp)) != RESULT_OK)
        return rc;
    // Check that the card now expects an application command
    if (!(resp & (1 << 5))) return rc;
    return RESULT_OK;
}

enum Storage::Result SDHC_SDIO::acmd(struct SDIO::Command command, uint32_t arg, void* response)
{
    // Make the card expectan application command
    enum Storage::Result rc = prepareACMD();
    if (rc != RESULT_OK) return rc;
    // Send the application command, and pass the result to the caller
    return sdio->command(command, arg, response);
}

enum Storage::Result SDHC_SDIO::modeSwitch(uint32_t argument, void* buf)
{
    enum Storage::Result rc;
    uint32_t resp;
    // Set up incoming data transfer
    if ((rc = sdio->startTransfer(buf, 64, { .blockSize = SDIO::BLOCKSIZE_64, .timeout = SDIO::TIMEOUT_128K,
                                             .direction = SDIO::DIRECTION_READ })) != RESULT_RESOURCE_BUSY)
        return rc;
    // Send switch command
    if ((rc = sdio->command({ .cmd = 6, .responseType = SDIO::RESPONSE_SHORT }, 0, &resp)) != RESULT_OK)
    {
        sdio->cancelTransfer();
        return rc;
    }
    // Check response
    if ((rc = checkResponse(resp)) != RESULT_OK)
    {
        sdio->cancelTransfer();
        return rc;
    }
    // Wait for data transfer to finish
    while ((rc = sdio->getTransferStatus()) == RESULT_RESOURCE_BUSY);
    // Return data transfer result to caller
    return rc;
}

enum Storage::Result SDHC_SDIO::reset()
{
    enum Storage::Result rc;
    uint32_t response[16];
    uint32_t ocr = 0x00ff8000;

    // Reset our state
    initialized = false;
    sdhc = false;
    pageCount = 0;
    rca = 0;

    // Configure controller for 400kHz 1-bit communications (card identification mode)
    if ((rc = sdio->configure(400000, 1)) != RESULT_OK) return rc;

    // CMD0: Reset the card
    if ((rc = sdio->command({ .cmd = 0, .responseType = SDIO::RESPONSE_NONE }, 0, NULL)) != RESULT_OK) return rc;

    // CMD8: Initialize and detect SD 2.0 compatible cards (required for SDHC)
    if (sdio->command({ .cmd = 8, .responseType = SDIO::RESPONSE_SHORT }, 0x1aa, &response) == RESULT_OK)
        // The card is SD 2.0 or newer, attempt SDHC init later
        if (response[0] == 0x1aa) ocr |= (1 << 30) | (sdio->supportsHighCurrent << 28);

    // ACMD41: Send operating conditions, agree on voltage range, wait for card initialization (up to 1 second)
    long timeout = TIMEOUT_SETUP(1000000);
    do rc = acmd({ .cmd = 41, .responseType = SDIO::RESPONSE_SHORT, .ignoreCRC = true, .ignoreOpcode = true },
                 ocr, response);
    while (!TIMEOUT_EXPIRED(timeout) && rc == RESULT_OK && !(response[0] & (1 << 31)));
    if (rc != RESULT_OK)
        do rc = sdio->command({ .cmd = 1, .responseType = SDIO::RESPONSE_SHORT, .ignoreCRC = true,
                                .ignoreOpcode = true }, ocr, response);
        while (!TIMEOUT_EXPIRED(timeout) && rc == RESULT_OK && !(response[0] & (1 << 31)));
    // Check for transfer failure
    if (rc != RESULT_OK) return rc;
    // Check for card still not being initialized after the timeout expires
    if (!(response[0] & (1 << 31))) return RESULT_TIMEOUT;
    // Check for SDHC support (requested via argument if card passed CMD8 init)
    if (response[0] & (1 << 30)) sdhc = true;

    // CMD2: Get CID (globally unique ID) of the first card on the bus
    if ((rc = sdio->command({ .cmd = 2, .responseType = SDIO::RESPONSE_LONG, .ignoreCRC = true, .ignoreOpcode = true },
                            0, response)) != RESULT_OK)
        return rc;

    // CMD3: Ask the card for its RCA (relative card address)
    if ((rc = sdio->command({ .cmd = 3, .responseType = SDIO::RESPONSE_SHORT }, 0, response)) != RESULT_OK) return rc;
    if ((rc = checkResponse(response[0] & 0xffff)) != RESULT_OK) return rc;
    rca = response[0] >> 16;

    // CMD9: Request CSD (card specific data, such as capacity)
    if ((rc = sdio->command({ .cmd = 9, .responseType = SDIO::RESPONSE_LONG, .ignoreCRC = true, .ignoreOpcode = true },
                            rca << 16, response)) != RESULT_OK)
        return rc;
    // Check if the CSD structure version is newer than this code
    if (response[0] >> 31) return RESULT_UNKNOWN_ERROR;
    // Calculate card capacity
    if (response[0] >> 30) pageCount = ((((response[1] & 0x3f) << 16) | (response[2] >> 16)) + 1) << 10;
    else pageCount = ((((response[1] & 0x3ff) << 2) | (response[2] >> 30)) + 1)
                  << (((response[2] >> 15) & 7) + ((response[1] >> 16) & 0xf) - 7);

    // CMD7: Select the card
    if ((rc = sdio->command({ .cmd = 7, .responseType = SDIO::RESPONSE_SHORT }, rca << 16, response)) != RESULT_OK)
        return rc;
    if ((rc = checkResponse(response[0])) != RESULT_OK) return rc;

    int speed = 0;
    int power = 0xf;
    // CMD6: Check if the card supports the switch command, and which modes are supported
    if (modeSwitch(0x00ffffff, response) == RESULT_OK)
    {
        // Card supports switch command, check if it supports high speed mode
        if (sdio->supportsHighSpeed && (response[3] & (1 << 17))) speed = 1;
        // Check if we can set a higher power limit
        if (sdio->supportsHighCurrent)
        {
            if (response[1] & (1 << 3)) power = 3;
            else if (response[1] & (1 << 2)) power = 2;
            else if (response[1] & (1 << 4)) power = 4;
            else if (response[1] & (1 << 1)) power = 1;
        }
        // Attempt to set the determined settings, check which speed was actually accepted by the card
        if ((rc = modeSwitch(0x00ff0ff0 | (power << 12) | speed, response)) != RESULT_OK) speed = 0;
        else speed = (response[4] >> 24) & 0xf;
    }

    int width = 1;
    do
    {
        // ACMD51: Request SCR (SD configuration register), determine maximum bus width
        if (prepareACMD() != RESULT_OK) break;
        if (sdio->startTransfer(&response[1], 8, { .blockSize = SDIO::BLOCKSIZE_8, .timeout = SDIO::TIMEOUT_128K,
                                                   .direction = SDIO::DIRECTION_READ }) != RESULT_RESOURCE_BUSY)
            break;
        if (sdio->command({ .cmd = 51, .responseType = SDIO::RESPONSE_SHORT }, 0, response) != RESULT_OK)
        {
            sdio->cancelTransfer();
            break;
        }
        if ((rc = checkResponse(response[0])) != RESULT_OK)
        {
            sdio->cancelTransfer();
            break;
        }
        while ((rc = sdio->getTransferStatus()) == RESULT_RESOURCE_BUSY);
        if (rc != RESULT_OK) break;
        // Check if the SCR structure version is newer than this code
        if (response[1] >> 28) break;
        // Check if 4 bit bus width is supported by both the SDIO host controller and the card
        if (sdio->maxWidth >= 4 && (response[1] & (1 << 10))) width = 4;

        // ACMD6: Set bus width (if anything goes wrong, just fall back to 1 bit width)
        if (acmd({ .cmd = 6, .responseType = SDIO::RESPONSE_SHORT }, width >> 1, response) != RESULT_OK
         || checkResponse(response[0]) != RESULT_OK)
            width = 1;
    }
    while (false);

    // Configure bus width and clock speed in the SDIO host controller
    sdio->configure(MIN(25000000 << speed, maxSpeed), width);

    // CMD16: Set block size (to make SD and SDHC cards behave similarly, mostly ignored by SDHC cards)
    if ((rc = sdio->command({ .cmd = 16, .responseType = SDIO::RESPONSE_SHORT }, 512, response)) != RESULT_OK)
        return rc;
    if ((rc = checkResponse(response[0])) != RESULT_OK) return rc;

    // We're finally done setting up this thing...
    initialized = true;
    return RESULT_OK;
}

enum Storage::Result SDHC_SDIO::getStatus()
{
    return initialized ? RESULT_OK : RESULT_INVALID_STATE;
}

enum Storage::Result SDHC_SDIO::read(uint32_t page, uint32_t len, void* buf)
{
    if (!initialized) return RESULT_INVALID_STATE;
    if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
    if (!len) return RESULT_OK;
    if (!sdhc) page *= 512;
    enum Storage::Result rc = RESULT_COMM_ERROR;
    int tries = retries;
    while (tries--)
    {
        uint32_t resp;
        // Prepare data transfer
        if ((rc = sdio->startTransfer(buf, len * 512, { .blockSize = SDIO::BLOCKSIZE_512, .timeout = SDIO::TIMEOUT_4M,
                                                        .direction = SDIO::DIRECTION_READ })) != RESULT_RESOURCE_BUSY)
            return rc;
        // CMD17/18: Read single/multiple block(s)
        typeof(SDIO::Command::cmd) cmd = len > 1 ? 18 : 17;
        if ((rc = sdio->command({ .cmd = cmd, .responseType = SDIO::RESPONSE_SHORT }, page, &resp)) != RESULT_OK)
        {
            sdio->cancelTransfer();
            return rc;
        }
        // Check response
        if ((rc = checkResponse(resp)) != RESULT_OK)
        {
            sdio->cancelTransfer();
            return rc;
        }
        // Wait for data transfer to finish
        while ((rc = sdio->getTransferStatus()) == RESULT_RESOURCE_BUSY);
        // CMD12: Stop data transfer
        if (len > 1) sdio->command({ .cmd = 12, .responseType = SDIO::RESPONSE_SHORT }, 0, &resp);
        if (rc != RESULT_COMM_ERROR) break;
    }
    return rc;
}

enum Storage::Result SDHC_SDIO::write(uint32_t page, uint32_t len, const void* buf)
{
    if (!initialized) return RESULT_INVALID_STATE;
    if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
    if (!len) return RESULT_OK;
    if (!sdhc) page *= 512;
    enum Storage::Result rc = RESULT_COMM_ERROR;
    uint32_t resp;
    // Attempt ACMD23 (set pre-erase count) for multi-block writes, we can ignore if this fails
    if (len > 1) acmd({ .cmd = 23, .responseType = SDIO::RESPONSE_SHORT }, len, &resp);
    int tries = retries;
    while (tries--)
    {
        // Prepare data transfer
        if ((rc = sdio->startTransfer(const_cast<void*>(buf), len * 512,
                                      { .blockSize = SDIO::BLOCKSIZE_512, .timeout = SDIO::TIMEOUT_4M,
                                        .direction = SDIO::DIRECTION_WRITE })) != RESULT_RESOURCE_BUSY)
            return rc;
        // CMD24/25: Write single/multiple block(s)
        typeof(SDIO::Command::cmd) cmd = len > 1 ? 25 : 24;
        if ((rc = sdio->command({ .cmd = cmd, .responseType = SDIO::RESPONSE_SHORT }, page, &resp)) != RESULT_OK)
        {
            sdio->cancelTransfer();
            return rc;
        }
        // Check response
        if ((rc = checkResponse(resp)) != RESULT_OK)
        {
            sdio->cancelTransfer();
            return rc;
        }
        // Wait for data transfer to finish
        while ((rc = sdio->getTransferStatus()) == RESULT_RESOURCE_BUSY);
        // CMD12: Stop data transfer
        if (len > 1) sdio->command({ .cmd = 12, .responseType = SDIO::RESPONSE_SHORT }, 0, &resp);
        if (rc != RESULT_COMM_ERROR) break;
    }
    return rc;
}

enum Storage::Result SDHC_SDIO::erase(uint32_t page, uint32_t len)
{
    if (!initialized) return RESULT_INVALID_STATE;
    if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
    if (!len) return RESULT_OK;
    if (!sdhc) page *= 512;
    enum Storage::Result rc = RESULT_COMM_ERROR;
    uint32_t resp;
    // CMD32: Set erase start address
    if ((rc = sdio->command({ .cmd = 32, .responseType = SDIO::RESPONSE_SHORT }, page, &resp)) != RESULT_OK) return rc;
    // Check response
    if ((rc = checkResponse(resp)) != RESULT_OK) return rc;
    // CMD33: Set erase end address
    if ((rc = sdio->command({ .cmd = 33, .responseType = SDIO::RESPONSE_SHORT }, page + len - 1, &resp)) != RESULT_OK)
        return rc;
    // Check response
    if ((rc = checkResponse(resp)) != RESULT_OK) return rc;
    // CMD38: Erase
    if ((rc = sdio->command({ .cmd = 38, .responseType = SDIO::RESPONSE_SHORT }, 0, &resp)) != RESULT_OK) return rc;
    // Check response
    if ((rc = checkResponse(resp)) != RESULT_OK) return rc;
    return rc;
}
