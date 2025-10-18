/*
 *      This file is part of the XenoReader library
 *      Copyright (c) 2025 JK
 *
 *      Licensed under the MIT License.
 *      See the included LICENSE file for license and attribution details.
 */
#pragma once
#include <stddef.h>
#include <stdint.h>

typedef struct XenoFile XenoFile;

/// @brief Returns the sector that contains the beginning of the file.
/// @param file File to get the sector of.
uint32_t XenoFile_GetSector(const XenoFile *file);

/// @brief Returns the size of the file passed.
/// @param file XenoFile to get size of.
int32_t XenoFile_GetSize(const XenoFile *file);