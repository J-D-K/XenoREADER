#include "XenoFile.h"

#define __XENO_INTERNAL__
#include "XenoFileInternal.h"

uint32_t xeno_file_get_sector(const XenoFile *file) { return file->sector; }

int32_t xeno_file_get_size(const XenoFile *file) { return file->size; }