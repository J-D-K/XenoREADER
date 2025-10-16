/*
 *      This file is part of the XenoReader library
 *      Copyright (c) 2025 JK
 *
 *      Licensed under the MIT License.
 *      See the included LICENSE file for license and attribution details.
 */

#pragma once
#include "Sector.h"

#include <stdbool.h>

// clang-format off
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct XenoReader XenoReader;

/// @brief Attempts to open a Xenogears disc image.
/// Does some verification using independent research I've done.
/// @param path Path to the image to attempt to open.
XenoReader *xeno_open_image(const char *path);

/// @brief Closes the reader passed.
/// @param reader Reader to close.
void xeno_close_image(XenoReader *reader);

/// @brief Returns the number of sectors in the image.
size_t xeno_get_sector_count(const XenoReader *reader);

/// @brief Seeks to the sector passed. Returns false on failure.
/// @param sector Disc sector to seek to.
/// @param reader Reader to seek with.
bool xeno_seek_sector(int sector, XenoReader *reader);

/// @brief Reads the current sector from the image into Sector out. Returns false on failure.
/// @param sectorOut Sector to read into.
/// @param reader XenoReader to read from.
bool xeno_read_sector(Sector *sectorOut, XenoReader *reader);

/// @brief Shortcut function to *reliable* determine the disc number.
/// @param reader Reader to read the disc from.
/// @return Disc 1 or 2. -1 on failure.
int xeno_get_disc_number(XenoReader *reader);

/// @brief Parses the table of contents in sector 24 and 24.
/// @param reader Reader to use to parse the contents of.
/// @return True on success. False on failure.
bool xeno_parse_table_of_contents(XenoReader *reader);

#ifdef __cplusplus
}
#endif
// clang-format on