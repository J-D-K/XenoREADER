#pragma once
#include "DynamicArray.h"

#include <stdint.h>

#define __XENO_INTERNAL__
#include "XenoFileInternal.h"

#ifdef __XENO_INTERNAL__

// clang-format off
struct XenoDir
{
    /// @brief This stores sub directory count.
    uint32_t dirCount;

    /// @brief This stores the file count.
    uint32_t fileCount;

    /// @brief Sub directory array.
    DynamicArray *subDirs;

    /// @brief File array.
    DynamicArray *files;
};
// clang-format on

#endif