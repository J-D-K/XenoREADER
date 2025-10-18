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

#include <assert.h>

// clang-format off

/// @brief This is the same as the Sector struct, but instead uses XenoFileData instead raw data.
typedef struct
{
    /// @brief Sync pattern. More or less useless.
    uint8_t syncPattern[12];

    /// @brief Sector header.
    SectorHeader header;

    /// @brief This is repeated.
    SectorSubHeader subHeader[2];

    /// @brief This makes this easier to work with.
    XenoFileData data;

    /// @brief Not sure if anything important is really here.
    uint8_t edcCrc[EDC_CRC_SIZE];
} XenoSector;

static_assert(sizeof(Sector) == SECTOR_SIZE, "XenoSector doesn't match the desired size!");

// clang-format on