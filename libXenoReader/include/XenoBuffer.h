/*
 *      This file is part of the XenoReader library
 *      Copyright (c) 2025 JK
 *
 *      Licensed under the MIT License.
 *      See the included LICENSE file for license and attribution details.
 */
#pragma once
#include <stdint.h>

// clang-format off
typedef struct
{
    /// @brief This is a pointer to a buffer containing the actual file data.
    unsigned char *data;

    /// @brief This is the size of the buffer to make it easier to access.
    int32_t size;
} XenoBuffer;

/// @brief Frees the buffer.
void XenoBuffer_Free(XenoBuffer *buffer);