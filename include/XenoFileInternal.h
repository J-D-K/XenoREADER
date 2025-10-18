#pragma once
#include <stdint.h>

#ifdef __XENO_INTERNAL__
// clang-format off
struct XenoFile
{
    /// @brief This is the index of the file within its directory.
    uint32_t index;

    /// @brief This is the sector where the file data begins.
    uint32_t sector;

    /// @brief This is the size of the file in bytes.
    int32_t  size;
};
#endif
// clang-format on