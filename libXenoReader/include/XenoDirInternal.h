/*
 *      This file is part of the XenoReader library
 *      Copyright (c) 2025 JK
 *
 *      Licensed under the MIT License.
 *      See the included LICENSE file for license and attribution details.
 */
#pragma once
#include "DynamicArray.h"

#include <stdint.h>

#define __XENO_INTERNAL__
#include "XenoFileInternal.h"

#ifdef __XENO_INTERNAL__

// clang-format off
struct XenoDir
{
    /// @brief Sub directory array.
    DynamicArray *subDirs;

    /// @brief File array.
    DynamicArray *files;
};
// clang-format on

#endif