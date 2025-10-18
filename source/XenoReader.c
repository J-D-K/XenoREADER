/*
 *      This file is part of the XenoReader library
 *      Copyright (c) 2025 JK
 *
 *      Licensed under the MIT License.
 *      See the included LICENSE file for license and attribution details.
 */

#include "XenoReader.h"

#include "DynamicArray.h"
#include "Sector.h"

#define __XENO_INTERNAL__
#include "XenoDirInternal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// clang-format off
typedef struct
{
    uint32_t sector;
    int32_t size;
} FilesystemEntry;

struct XenoReader
{
    /// @brief Pointer the the image being read.
    FILE *image;

    /// @brief Stores the number of sectors the image has.
    size_t sectorCount;

    /// @brief Stores the disc number from allocation verification.
    int discNumber;

    /// @brief This is the root of the filesystem.
    XenoDir *root;
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

// Defined at bottom.
static bool read_array_to_directory(XenoDir *dir, DynamicArray *array, int *index);
static void free_directory_tree(XenoDir *dir);

XenoReader *XenoReader_Open(const char *path)
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
    reader->root        = XenoDir_Create();
    if (!reader->root) { goto Label_cleanup; }

    // Seek to where the filesystem/table is.
    if (!XenoReader_SeekToSector(reader, 24)) { goto Label_cleanup; }

    // The table takes up 16 sectors. We're going to buffer them all.
    const int tableBufferSize  = 16 * DATA_SIZE;
    unsigned char *tableBuffer = malloc(tableBufferSize);
    for (int i = 0; i < 16; i++)
    {
        Sector sector = {0};
        if (!XenoReader_ReadRawSector(reader, &sector)) { goto Label_cleanup; }

        // Copy it the the buffer.
        memcpy(&tableBuffer[i * DATA_SIZE], sector.data, DATA_SIZE);
    }

    // We're going to read all of the entries to this. This initial capacity is to prevent reallocations.
    DynamicArray *fsArray = DynamicArray_Create(sizeof(FilesystemEntry), 4096);
    if (!fsArray) { goto Label_cleanup; }

    for (int i = 0; i < tableBufferSize; i += 7)
    {
        // What we're reading to.
        uint32_t sector = 0; // This is actually stored as a 24bit value.
        int32_t size    = 0;

        // Positions in the buffer.
        const void *sectorPosition = &tableBuffer[i];
        const void *sizePosition   = &tableBuffer[i + 3];

        // Copy them.
        memcpy(&sector, sectorPosition, 3);
        memcpy(&size, sizePosition, 4);
        // If it has no sector, skip it. There are files that have 0 as a size. Not sure what purpose that serves yet.
        if (sector == 0) { continue; }

        // Add it to our array.
        FilesystemEntry *entry = (FilesystemEntry *)DynamicArray_New(fsArray);
        if (!entry) { goto Label_cleanup; }

        entry->sector = sector;
        entry->size   = size;
    }

    // This isn't needed anymore.
    free(tableBuffer);

    // We need to start the root here. The rest are recursive.
    int index               = 0;
    const int fsArrayLength = DynamicArray_GetLength(fsArray);
    while (index < fsArrayLength)
    {
        const FilesystemEntry *entry = DynamicArray_GetElementAt(fsArray, index);
        // Negative size denotes a "directory".
        if (entry->size < 0) { read_array_to_directory(reader->root, fsArray, &index); }
        else {
            // Add a new file.
            XenoFile *file = (XenoFile *)DynamicArray_New(reader->root->files);

            // Assign.
            file->sector = entry->sector;
            file->size   = entry->size;
            ++index;
        }
    }

    DynamicArray_Free(fsArray);

    return reader;

Label_cleanup:
    if (tableBuffer) { free(tableBuffer); }
    if (reader->root) { free_directory_tree(reader->root); }
    if (reader) { free(reader); }
    if (image) { fclose(image); }

    return NULL;
}

void XenoReader_Close(XenoReader *reader)
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

int XenoReader_GetDiscNumber(const XenoReader *reader) { return reader->discNumber; }

size_t XenoReader_GetSectorCount(const XenoReader *reader) { return reader->sectorCount; }

bool XenoReader_SeekToSector(XenoReader *reader, size_t sectorNumber)
{
    if (sectorNumber >= reader->sectorCount) { return false; }

    return fseek(reader->image, sectorNumber * SECTOR_SIZE, SEEK_SET) == 0;
}

bool XenoReader_ReadRawSector(XenoReader *reader, Sector *sectorOut)
{
    return fread(sectorOut, 1, SECTOR_SIZE, reader->image) == SECTOR_SIZE;
}

bool XenoReader_ReadXenoSector(XenoReader *reader, XenoSector *sectorOut)
{
    return fread(sectorOut, 1, SECTOR_SIZE, reader->image) == SECTOR_SIZE;
}

XenoDir *XenoReader_GetRootDirectory(XenoReader *reader) { return reader->root; }

static bool read_array_to_directory(XenoDir *dir, DynamicArray *array, int *index)
{
    // Grab the entry and ensure it's an array.
    const FilesystemEntry *entry = (const FilesystemEntry *)DynamicArray_GetElementAt(array, *index);
    if (entry->size >= 0) { return false; }

    // Increment the index and invert the size to get the total entries.
    ++*index;
    const int entryCount = -(entry->size);

    // Calculate where the end of the directory is.
    const int directoryEnd = *index + entryCount;

    // Add the new subDir to the incoming dir since directory is confirmed.
    XenoDir *subDir = (XenoDir *)DynamicArray_New(dir->subDirs);
    subDir->subDirs = DynamicArray_Create(sizeof(XenoDir), 32);
    subDir->files   = DynamicArray_Create(sizeof(XenoFile), 32);

    // Loop. This is recursive.
    for (; *index < directoryEnd;)
    {
        const FilesystemEntry *subEntry = (const FilesystemEntry *)DynamicArray_GetElementAt(array, *index);

        // Same as above.
        if (subEntry->size < 0)
        {
            if (!read_array_to_directory(subDir, array, index)) { return false; }
        }
        else {
            // Create new file entry.
            XenoFile *file = (XenoFile *)DynamicArray_New(subDir->files);

            // Copy the array values to the new file.
            file->sector = subEntry->sector;
            file->size   = subEntry->size;

            // Increment index.
            ++*index;
        }
    }

    return true;
}

static void free_directory_tree(XenoDir *dir)
{
    if (!dir) { return; }

    if (dir->files) { DynamicArray_Free(dir->files); }

    if (dir->subDirs)
    {
        const int dirCount = DynamicArray_GetLength(dir->subDirs);
        for (int i = 0; i < dirCount; i++)
        {
            free_directory_tree((XenoDir *)DynamicArray_GetElementAt(dir->subDirs, i));
        }
    }

    DynamicArray_Free(dir);
}