/*
 *      This file is part of the XenoReader library
 *      Copyright (c) 2025 JK
 *
 *      Licensed under the MIT License.
 *      See the included LICENSE file for license and attribution details.
 */

#include "BootRecord.h"

BootRecord *boot_record_from_sector(Sector *sector)
{
    if (!sector) { return NULL; }

    return (BootRecord *)sector->data;
}