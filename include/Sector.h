/*
 *      This file is part of the XenoReader library
 *      Copyright (c) 2025 JK
 *
 *      Licensed under the MIT License.
 *      See the included LICENSE file for license and attribution details.
 */

#pragma once

#include "defines.h"

#include <assert.h>
#include <stdint.h>

// This file contains the structs for reading a sector from a Xenogears image. This should apply to a lot of PSone games
// though.

// clang-format off
#ifdef __cplusplus
extern "C"
{
#endif 

/// @brief This is the header.
typedef struct
{
    uint8_t minute;
    uint8_t second;
    uint8_t frame;
    uint8_t mode;
} SectorHeader;

/// @brief This is the subheader struct. Every sector has this repeated.
typedef struct
{
    uint8_t fileNumber;
    uint8_t channelNumber;
    uint8_t subMode;
    uint8_t codingInfo;
} SectorSubHeader;

typedef struct
{
    /// @brief This is the sync pattern. It's more or less useless.
    uint8_t syncPattern[12];
    
    /// @brief The actual sector header.
    SectorHeader header;
    
    /// @brief These are the sub headers.
    SectorSubHeader subHeader[2];
    
    /// @brief This is the raw sector data.
    uint8_t data[DATA_SIZE];
    
    /// @brief I'm not entirely sure what is here. I don't really care about it either unless it becomes a problem. Then I'll care.
    uint8_t edcCrc[EDC_CRC_SIZE];
} Sector;

static_assert(sizeof(Sector) == SECTOR_SIZE, "Sector struct does not match sector size!");

#ifdef __cplusplus
}
#endif
// clang-format on