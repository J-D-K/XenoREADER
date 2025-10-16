#include "XenoReader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// clang-format off
struct XenoReader
{
    /// @brief Pointer the the image being read.
    FILE *image;

    /// @brief Stores the number of sectors the image has.
    size_t sectorCount;

    /// @brief Stores the disc number from allocation verification.
    int discNumber;
};
// clang-format on

// These are to verify the image before continuing.

// These are the number of sectors each disc has.
static const size_t DISC_1_SECTOR_COUNT = 305586;
static const size_t DISC_2_SECTOR_COUNT = 292815;

// This is the sector that contains the string to determine disc number.
const size_t DISC_SECTOR = 23;

// These are the strings that can be used to *RELIABLY* determine which disc is being targetted. This string in
// contained in sector 23.
#define DISC_STRING_LENGTH 16
static const char *DISC_1_STRING = "DS01_XENOGEARS";
static const char *DISC_2_STRING = "DS02_XENOGEARS";

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

    // Seek to sector 23 and read it to check which disc we're working with for sure.
    Sector discSector;
    const bool seek       = fseek(image, DISC_SECTOR * SECTOR_SIZE, SEEK_SET) == 0;
    const bool sectorRead = seek && fread(&discSector, 1, SECTOR_SIZE, image) == SECTOR_SIZE;
    if (!sectorRead) { goto Label_cleanup; }

    // We're going to copy the first part of the sector data and compare the string to help determine disc number.
    char discBuffer[DISC_STRING_LENGTH] = {0};

    // Copy the string to our local buffer. Compare that buffer to see what disc we're using.
    strncpy(discBuffer, discSector.data, DISC_STRING_LENGTH);

    bool discOne       = strcmp(DISC_1_STRING, discBuffer) == 0;
    const bool discTwo = strcmp(DISC_2_STRING, discBuffer) == 0;
    if (!discOne && !discTwo) { goto Label_cleanup; }

    // I wanted as much of the verification out of the way as possible before allocating this.
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