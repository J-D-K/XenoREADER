#include "XenoReader.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[])
{
    XenoReader *xenoReader = XenoReader_Open(argv[1]);
    if (!xenoReader)
    {
        printf("xenoReader is NULL!\n");
        return -1;
    }

    XenoReader_LoadProcessFilesystem(xenoReader);

    XenoReader_Close(xenoReader);
}