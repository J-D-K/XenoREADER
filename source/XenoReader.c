/*
 *      This file is part of the XenoReader library
 *      Copyright (c) 2025 JK
 *
 *      Licensed under the MIT License.
 *      See the included LICENSE file for license and attribution details.
 */

#include "XenoReader.h"
#include "Sector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// clang-format off
typedef struct
{
    uint32_t sector; // This is really stored as 24 bits. This seems to be the only important part.
    int32_t size; // I'm not entirely sure this is the size of anything. Nothing lines up if you use this.
} XenoEntry;

_Static_assert(sizeof(XenoEntry) == 8, "XenoEntry does not match the desired size!");

struct XenoReader
{
    /// @brief Pointer the the image being read.
    FILE *image;

    /// @brief Stores the number of sectors the image has.
    size_t sectorCount;

    /// @brief Stores the disc number from allocation verification.
    int discNumber;

    /// @brief Array of entries.
    XenoEntry **fileEntries;

    /// @brief This is the number of file entries read.
    size_t entryCount;
};
// clang-format on

// The following consts and values are used to verify the image before returning the struct.

// This is the size of the buffer used to pull strings from sectors to verify the image.
#define STRING_BUFFER_SIZE 32

// These are the number of sectors each disc has.
static const size_t DISC_1_SECTOR_COUNT = 305586;
static const size_t DISC_2_SECTOR_COUNT = 292815;

// This is a way to determine that the image is a valid Xenogears image. This uses the boot record.
/// @brief This is the boot record sector.
static const size_t BOOT_RECORD_SECTOR = 16;

/// @brief This is the offset where XENOGEARS appears in the boot record.
static const size_t BOOT_RECORD_OFFSET = 0x28;

/// @brief This is the string that appears in the boot record.
static const char *BOOT_RECORD_STRING = "XENOGEARS";

/// @brief This is the size of the string above.
static const size_t XENOGEARS_STRING_LENGTH = 9;

/// @brief This is the sector that contains the string to identify the disc number.
static const size_t DISC_IDENTIFICATION_SECTOR = 23;

/// @brief These are the strings that can be used to *RELIABLY* determine which disc is being targetted. These strings
/// are contained in sector 23.
static const char *DISC_1_STRING = "DS01_XENOGEARS";
static const char *DISC_2_STRING = "DS02_XENOGEARS";

/// @brief This is the length of the strings above.
static const int DISC_STRING_LENGTH = 14;

XenoReader *xeno_open_image(const char *path)
{
    // This will pretty much
    struct stat fileStat;
    if (stat(path, &fileStat) != 0) { return NULL; };

    // Check the sector count first.
    const size_t sectorCount = fileStat.st_size / SECTOR_SIZE;
    if (sectorCount != DISC_1_SECTOR_COUNT && sectorCount != DISC_2_SECTOR_COUNT) { return NULL; }

    // Try to open the image for reading.
    FILE *image = fopen(path, "rb");
    if (!image) { goto Label_cleanup; }

    // This is scoped so that it doesn't hang around in the stack after it's not needed.
    {
        // Here, we read the boot sector and check for the XENOGEARS string.
        Sector bootSector;
        const bool bootSeek = fseek(image, BOOT_RECORD_SECTOR * SECTOR_SIZE, SEEK_SET) == 0;
        const bool bootRead = bootSeek && fread(&bootSector, 1, SECTOR_SIZE, image) == SECTOR_SIZE;
        if (!bootRead) { goto Label_cleanup; }

        // This is used to pull and copy the "header(?)" from the boot sector.
        char headerBuffer[STRING_BUFFER_SIZE] = {0};

        // Copy the string into the buffer and .
        strncpy(headerBuffer, &bootSector.data[BOOT_RECORD_OFFSET], XENOGEARS_STRING_LENGTH);

        const bool isXenogears = strcmp(headerBuffer, BOOT_RECORD_STRING) == 0;
        if (!isXenogears) { goto Label_cleanup; }
    }

    // Same as above. Scoped.
    bool discOne;
    bool discTwo;
    {
        // Seek to sector 23 and read it to check which disc we're working with for sure.
        Sector discSector;
        bool seek             = fseek(image, DISC_IDENTIFICATION_SECTOR * SECTOR_SIZE, SEEK_SET) == 0;
        const bool sectorRead = seek && fread(&discSector, 1, SECTOR_SIZE, image) == SECTOR_SIZE;
        if (!sectorRead) { goto Label_cleanup; }

        // This is used to read the disc identification string in sector 23.
        char discBuffer[STRING_BUFFER_SIZE] = {0};

        // Copy the string to our local buffer. Compare that buffer to see what disc we're using.
        strncpy(discBuffer, discSector.data, DISC_STRING_LENGTH);
        discOne = strcmp(DISC_1_STRING, discBuffer) == 0;
        discTwo = strcmp(DISC_2_STRING, discBuffer) == 0;
        if (!discOne && !discTwo) { goto Label_cleanup; }
    }

    // Seek back to the beginning of the image for consistency.
    const bool beginningSeek = fseek(image, 0, SEEK_SET) == 0;
    if (!beginningSeek) { goto Label_cleanup; }

    // I wanted all of the validation done before this to make this less of a pain.
    XenoReader *reader = (XenoReader *)malloc(sizeof(XenoReader));
    if (!reader) { goto Label_cleanup; }

    reader->image       = image;
    reader->sectorCount = sectorCount;
    reader->discNumber  = discOne ? 1 : 2;

    return reader;

Label_cleanup:
    if (image) { fclose(image); }

    return NULL;
}

void xeno_close_image(XenoReader *reader)
{
    // Bail if NULL is passed.
    if (!reader) { return; }

    // Only try to close the file if it's actually open.
    if (reader->image)
    {
        fclose(reader->image);
        reader->image = NULL;
    }

    // Free the memory.
    free(reader);
}

size_t xeno_get_sector_count(const XenoReader *reader) { return reader->sectorCount; }

bool xeno_seek_sector(int sector, XenoReader *reader)
{
    if (!reader || !reader->image) { return false; }

    return fseek(reader->image, SECTOR_SIZE * sector, SEEK_SET) == 0;
}

bool xeno_read_sector(Sector *sectorOut, XenoReader *reader)
{
    if (!reader || !reader->image) { return false; }

    return fread(sectorOut, 1, SECTOR_SIZE, reader->image) == SECTOR_SIZE;
}

int xeno_get_disc_number(XenoReader *reader) { return reader->discNumber; }

bool xeno_parse_table_of_contents(XenoReader *reader) {}