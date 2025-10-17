#include "XenoFileData.h"

XenoFileData *xeno_file_data_from_sector(const Sector *sector)
{
    return (XenoFileData *)sector->data;
}