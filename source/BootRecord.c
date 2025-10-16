#include "BootRecord.h"

BootRecord *boot_record_from_sector(Sector *sector)
{
    if (!sector) { return NULL; }

    return (BootRecord *)sector->data;
}