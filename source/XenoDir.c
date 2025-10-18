#include "XenoDir.h"

#define __XENO_INTERNAL__
#include "XenoDirInternal.h"

uint32_t xeno_dir_get_sub_dir_count(const XenoDir *dir) { return dir->dirCount; }

uint32_t xeno_dir_get_file_count(const XenoDir *dir) { return dir->fileCount; }

XenoDir *xeno_dir_get_dir_at(const XenoDir *dir, int index)
{
    if (index < 0 || index >= (int)dir->dirCount) { return NULL; }

    return (XenoDir *)dynamic_array_get_element_at(dir->subDirs, index);
}

XenoFile *xeno_dir_get_file_at(const XenoDir *dir, int index)
{
    if (index < 0 || index >= (int)dir->fileCount) { return NULL; }

    return (XenoFile *)dynamic_array_get_element_at(dir->files, index);
}