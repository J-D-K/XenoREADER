/*
 *      This file is part of the XenoReader library
 *      Copyright (c) 2025 JK
 *
 *      Licensed under the MIT License.
 *      See the included LICENSE file for license and attribution details.
 */

#pragma once
#include "Sector.h"
#include "XenoSector.h"

#include <stdbool.h>

// clang-format off
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct XenoReader XenoReader;

/// @brief Attempts to open a Xenogears disc image.
/// @param path Path to the image to attempt to open.
/// @note Verifies the image in multiple ways before returning a XenoReader.
XenoReader *XenoReader_Open(const char *path);

/// @brief Closes the reader passed.
/// @param reader Reader to close.
void XenoReader_Close(XenoReader *reader);

/// @brief Shortcut function to return the disc number fetched during allocation.
/// @param reader Reader to get disc number from.
/// @return Disc 1 or 2. -1 on failure.
int XenoReader_GetDiscNumber(const XenoReader *reader);

/// @brief Returns the total sector count of the image.
/// @param reader Reader to get count of.
/// @return Sector count.
size_t XenoReader_GetSectorCount(const XenoReader *reader);

/// @brief Seeks to the sector passed.
/// @param reader Reader to seek with.
/// @param sectorNumber Sector number to seek to.
/// @return True on success. False on failure.
bool XenoReader_SeekToSector(XenoReader *reader, size_t sectorNumber);

/// @brief Reads the next sector to the Sector struct passed.
/// @param reader XenoReader to read from.
/// @param sectorOut Sector to read data into.
/// @return True on success. False on failure.
bool XenoReader_ReadRawSector(XenoReader *reader, Sector *sectorOut);

/// @brief Reads the current sector into a XenoSector for easier
/// @param reader Reader to read the sector with.
/// @param sectorOut XenoSector struct to read to.
/// @return True on success. False on failure.
bool XenoReader_ReadXenoSector(XenoReader *reader, XenoSector *sectorOut);

/// @brief Loads and processes the hidden filesystem in the Xenogears image.
/// @param reader Reader to process the filesystem for.
/// @return True on success. False on failure.
bool XenoReader_LoadProcessFilesystem(XenoReader *reader);

/// @brief Returns the root "hidden" directory.
/// @param reader Reader to return the root filesystem of.
XenoDir *XenoReader_GetRootDirectory(XenoReader *reader);

#ifdef __cplusplus
}
#endif
// clang-format on