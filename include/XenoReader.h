/*
 *      This file is part of the XenoReader library
 *      Copyright (c) 2025 JK
 *
 *      Licensed under the MIT License.
 *      See the included LICENSE file for license and attribution details.
 */

#pragma once
#include "Sector.h"
#include "XenoFileData.h"

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

/// @brief Shortcut function to *reliable* determine the disc number.
/// @param reader Reader to read the disc from.
/// @return Disc 1 or 2. -1 on failure.
int xeno_get_disc_number(XenoReader *reader);

/// @brief Parses the table of contents in sector 24 (possible 25 too?)
/// @param reader Reader to use to parse the contents of.
/// @return True on success. False on failure.
bool xeno_parse_table_of_contents(XenoReader *reader);

/// @brief Goes to the first sector containing the first "directory" (They don't really exist).
/// @param reader Reader to use.
/// @return True on success. False on failure.
bool xeno_go_to_first_directory(XenoReader *reader);

/// @brief Goes to the next sector containing the next "directory".
/// @param reader Reader to use.
/// @return True on success. False on failure.
bool xeno_go_to_next_directory(XenoReader *reader);


#ifdef __cplusplus
}
#endif
// clang-format on