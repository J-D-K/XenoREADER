/*
 *      This file is part of the XenoReader library
 *      Copyright (c) 2025 JK
 *
 *      Licensed under the MIT License.
 *      See the included LICENSE file for license and attribution details.
 */
#pragma once
#include "XenoFile.h"

#include <stdint.h>

typedef struct XenoDir XenoDir;

/// @brief Returns the subdirectory count.
/// @param dir Directory to get subdirectory count of.
uint32_t XenoDir_GetDubDirCount(const XenoDir *dir);

/// @brief Returns the count of files contained within the directory.
/// @param dir Dir to get file count of.
uint32_t XenoDir_GetFileCount(const XenoDir *dir);

/// @brief Returns the subdirectory at the index passed. NULL otherwise.
/// @param dir Directory get the subdirectory from.
/// @param index Index of the subdirectory.
XenoDir *XenoDir_GetDirAt(const XenoDir *dir, int index);

/// @brief Returns the file at the index passed. NULL otherwise.
/// @param dir Directory to pull the file data from.
/// @param index Index of the file.
XenoFile *XenoDir_GetFileAt(const XenoDir *dir, int index);
