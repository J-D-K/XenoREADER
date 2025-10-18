/*
 *      This file is part of the XenoReader library
 *      Copyright (c) 2025 JK
 *
 *      Licensed under the MIT License.
 *      See the included LICENSE file for license and attribution details.
 */
#pragma once
#include "Sector.h"

#include <assert.h>
#include <stdint.h>

// clang-format off
// As of now, this struct is entirely made upon educated guesses on what I've observed.

// This is the actual amount of data held per sector. The first 32 bytes seem to be a header.
#define XENO_DATA_SIZE 2016

typedef struct
{
    // I'm not entirely sure what this is yet. It doesn't change for most sectors I've looked at.
    uint32_t magicHeader;
    
    // This seems to increase with every sector that's part of the same file.
    uint16_t part;

    // This seems to stay the same for the most part and seems to line up with the number of sectors that make up the file.
    // Some of the final parts of the file seem to decompression data?
    uint16_t total;

    // This seems to stay the same per file, but increments by one when the total is met? 
    uint16_t dirIndex;

    uint16_t unk[9];


    // // Not sure about these yet.
    // uint16_t unkA;
    // uint16_t unkB;
    // uint16_t unkC;
    
    // // These two don't seem to change between files.
    // uint16_t unkD;
    // uint16_t unkE;

    // // No guess as of now.
    // uint16_t unkF;
    // uint16_t unkG;

    // // These are always 2 in what I've looked at?
    // uint16_t unkH;
    // uint16_t unkI;

    // Seems to always be 0?
    uint32_t unkJ;
} XenoDataHeader;

static_assert(sizeof(XenoDataHeader) == 32, "XenoDataHeader size doesn't match intended size!");

typedef struct
{
    // This is the header and contains data about the data.
    XenoDataHeader header;

    // This seems to be actual data contained in the sector.
    uint8_t data[XENO_DATA_SIZE];
} XenoFileData;

static_assert(sizeof(XenoFileData) == 2048, "XenoFileData doesn't match intended size!");
// clang-format on