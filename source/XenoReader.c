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
#include "XenoDir.h"

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

    /// @brief This is the root directory of the hidden disc filesystem.
    XenoDir *rootDir;
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

// Defined at bottom. This is needed for recursive FS layout generation.

/// @brief Adds a subdirectory to the directory passed.
/// @param dir Directory to add the entry to.
/// @param fsArray Pointer to dynamic array containing the filesystem entries.
/// @param index Index of the entry containing the data to create a subdirectory with.
/// @return True on success. False on failure.
static bool add_sub_directory_to_directory(XenoDir *dir, const DynamicArray *fsArray, int *index);

static void debug_print_XenoDir(XenoDir *dir);

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
    reader->rootDir     = malloc(sizeof(XenoDir));
    if (!reader->rootDir) { goto Label_cleanup; }

    // We know these become huge-ish
    reader->rootDir->subDirs = DynamicArray_Create(sizeof(XenoDir), 512);
    reader->rootDir->files   = DynamicArray_Create(sizeof(XenoFile), 512);

    return reader;

Label_cleanup:
    if (image) { fclose(image); }
    if (reader) { free(reader); }

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

bool XenoReader_LoadProcessFilesystem(XenoReader *reader)
{
    // Start by seeking to sector 24.
    if (!XenoReader_SeekToSector(reader, 24)) { return false; }

    // The table is 16 sectors long. We need a buffer to load and store that.
    const size_t fsBufferLength = 16 * DATA_SIZE;
    unsigned char *fsBuffer     = malloc(fsBufferLength);

    // Loop and read the entire file table to the buffer.
    for (size_t i = 0; i < 16; i++)
    {
        Sector sector;
        if (!XenoReader_ReadRawSector(reader, &sector)) { return false; };

        memcpy(&fsBuffer[i * DATA_SIZE], sector.data, DATA_SIZE);
    }

    // Going to read the entries into one of these since it's easier.
    DynamicArray *fsArray =
        DynamicArray_Create(sizeof(FilesystemEntry),
                            4096); // This array is really large. The initial is to cut down on allocations.
    for (size_t i = 0; i < fsBufferLength; i += 7)
    {
        // Going to copy/read these first to ensure we only allocated what's needed.
        uint32_t sector = 0;
        int32_t size    = 0;

        // Copy from the buffer.
        memcpy(&sector, &fsBuffer[i], 3);
        memcpy(&size, &fsBuffer[i + 3], 4);
        if (sector == 0 || size == 0) { continue; }

        // Create a new entry.
        FilesystemEntry *newEntry = (FilesystemEntry *)DynamicArray_New(fsArray);

        // Assign
        newEntry->sector = sector;
        newEntry->size   = size;
    }

    // Fill out the root. The add_sub_directory_to_directory is recursive, but the root cannot really use that for now?
    XenoDir *rootDir        = reader->rootDir;
    const int fsArrayLength = DynamicArray_GetLength(fsArray);
    for (int index = 0; index < fsArrayLength;)
    {
        const FilesystemEntry *entry = DynamicArray_GetElementAt(fsArray, index);
        if (entry->size < 0)
        {
            ++rootDir->dirCount;
            add_sub_directory_to_directory(reader->rootDir, fsArray, &index);
        }
        else
        {
            ++rootDir->fileCount;
            XenoFile *newFile = (XenoFile *)DynamicArray_New(reader->rootDir->files);
            newFile->sector   = entry->sector;
            newFile->size     = entry->size;
            ++index;
        }
    }

    // We're done with this now.
    free(fsBuffer);

    debug_print_XenoDir(reader->rootDir);

    DynamicArray_Free(fsArray);
}

static bool add_sub_directory_to_directory(XenoDir *dir, const DynamicArray *fsArray, int *index)
{
    // Grab the entry and ensure it's a directory.
    const FilesystemEntry *entry = (const FilesystemEntry *)DynamicArray_GetElementAt(fsArray, *index);
    if (entry->size >= 0) { return false; }

    // This will align to the first file inside the directory.
    ++*index;

    // Allocate the new sub directory.
    XenoDir *subDir = (XenoDir *)DynamicArray_New(dir->subDirs);
    if (!subDir) { return false; }

    // Sub directory needs these too.
    subDir->subDirs = DynamicArray_Create(sizeof(XenoDir), 32);
    subDir->files   = DynamicArray_Create(sizeof(XenoFile), 32);

    // Invert the size for directories.
    const int entryCount = -entry->size;
    const int end        = *index + entryCount;

    for (int i = *index; i < end;)
    {
        const FilesystemEntry *subEntry = (const FilesystemEntry *)DynamicArray_GetElementAt(fsArray, i);
        if (!subEntry) { continue; }

        if (subEntry->size < 0)
        {
            ++subDir->dirCount;
            add_sub_directory_to_directory(subDir, fsArray, index);

            i += entryCount;
            index += entryCount;
        }
        else
        {
            ++subDir->fileCount;
            XenoFile *newFile = (XenoFile *)DynamicArray_New(subDir->files);
            newFile->sector   = subEntry->sector;
            newFile->size     = subEntry->size;

            *index++;
            i++;
        }
    }
    return true;
}

void debug_print_XenoDir(XenoDir *dir)
{
    printf("debug_print_XenoDir\n");

    const size_t dirCount  = XenoDir_GetDubDirCount(dir);
    const size_t fileCount = XenoDir_GetFileCount(dir);
    printf("dirCount = %u\nfileCount = %u\n", dirCount, fileCount);

    for (size_t i = 0; i < dirCount; i++)
    {
        printf("SubDir %u:\n", i);

        XenoDir *subDir = XenoDir_GetDirAt(dir, i);
        debug_print_XenoDir(subDir);
    }

    // for (size_t i = 0; i < fileCount; i++) { printf("\t\nFile %u\n", i); }
}
