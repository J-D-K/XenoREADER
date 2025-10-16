/*
 *      This file is part of the XenoReader library
 *      Copyright (c) 2025 JK
 *
 *      Licensed under the MIT License.
 *      See the included LICENSE file for license and attribution details.
 */

#pragma once

#include "Sector.h"
#include "defines.h"

#include <assert.h>
#include <stdint.h>

// clang-format off
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    uint8_t type;
    char identifier[5];
    uint8_t version;
    char sysIdentifier[32];
    char bootIdentifier[32];
    char systemUse[1977];
} BootRecord;

/// @brief Returns a pointer to the BootRecord from the sector passed.
/// @param sector Sector to get the boot record from.
/// @return Pointer to boot record on success. NULL on failure.
BootRecord *boot_record_from_sector(Sector *sector);

#ifdef __cplusplus
}
#endif
// clang-format on