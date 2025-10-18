#include "XenoDir.h"

#include <stdlib.h>

#define __XENO_INTERNAL__
#include "XenoDirInternal.h"

XenoDir *XenoDir_Create()
{
    XenoDir *dir = malloc(sizeof(XenoDir));
    if (!dir) { return NULL; }

    // Both of these need to be allocated too.
    dir->subDirs = DynamicArray_Create(sizeof(XenoDir), 32);
    dir->files   = DynamicArray_Create(sizeof(XenoFile), 32);
    if (!dir->subDirs || !dir->files) { goto Label_cleanup; }

    // Good to go?
    return dir;

Label_cleanup:
    if (dir) { XenoDir_Free(dir); }

    return NULL;
}

void XenoDir_Free(XenoDir *dir)
{
    if (!dir) { return; }
    if (dir->subDirs) { DynamicArray_Free(dir->subDirs); }
    if (dir->files) { DynamicArray_Free(dir->files); }

    free(dir);
}

uint32_t XenoDir_GetDubDirCount(const XenoDir *dir) { return DynamicArray_GetLength(dir->subDirs); }

uint32_t XenoDir_GetFileCount(const XenoDir *dir) { return DynamicArray_GetLength(dir->files); }

XenoDir *XenoDir_GetDirAt(const XenoDir *dir, int index)
{
    if (index < 0 || index >= (int)DynamicArray_GetLength(dir->subDirs)) { return NULL; }

    return (XenoDir *)DynamicArray_GetElementAt(dir->subDirs, index);
}

XenoFile *XenoDir_GetFileAt(const XenoDir *dir, int index)
{
    if (index < 0 || index >= (int)DynamicArray_GetLength(dir->files)) { return NULL; }

    return (XenoFile *)DynamicArray_GetElementAt(dir->files, index);
}