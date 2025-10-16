#pragma once
#include <stddef.h>

/// @brief This is the size of the sectors used in Mode 2 PS1 games.
#define SECTOR_SIZE 2352

/// @brief This is the size of the data portion of the sector.
#define DATA_SIZE 2048

// I'm going off some documents on the ISO-9660 standard for this one.
// I'm not sure what's contained in here and it needs more research.
#define EDC_CRC_SIZE 280