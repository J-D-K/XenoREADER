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
uint32_t xeno_dir_get_sub_dir_count(const XenoDir *dir);

/// @brief Returns the count of files contained within the directory.
/// @param dir Dir to get file count of.
uint32_t xeno_dir_get_file_count(const XenoDir *dir);

/// @brief Returns the subdirectory at the index passed. NULL otherwise.
/// @param dir Directory get the subdirectory from.
/// @param index Index of the subdirectory.
XenoDir *xeno_dir_get_dir_at(const XenoDir *dir, int index);

/// @brief Returns the file at the index passed. NULL otherwise.
/// @param dir Directory to pull the file data from.
/// @param index Index of the file.
XenoFile *xeno_dir_get_file_at(const XenoDir *dir, int index);
