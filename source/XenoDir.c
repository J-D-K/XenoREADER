#include "XenoDir.h"

#define __XENO_INTERNAL__
#include "XenoDirInternal.h"

uint32_t XenoDir_GetDubDirCount(const XenoDir *dir) { return dir->dirCount; }

uint32_t XenoDir_GetFileCount(const XenoDir *dir) { return dir->fileCount; }

XenoDir *XenoDir_GetDirAt(const XenoDir *dir, int index)
{
    if (index < 0 || index >= (int)dir->dirCount) { return NULL; }

    return (XenoDir *)DynamicArray_GetElementAt(dir->subDirs, index);
}

XenoFile *XenoDir_GetFileAt(const XenoDir *dir, int index)
{
    if (index < 0 || index >= (int)dir->fileCount) { return NULL; }

    return (XenoFile *)DynamicArray_GetElementAt(dir->files, index);
}