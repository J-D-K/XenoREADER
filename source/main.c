#include "XenoReader.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[])
{
    XenoReader *xenoReader = xeno_open_image(argv[1]);
    if (!xenoReader)
    {
        printf("xenoReader is NULL!\n");
        return -1;
    }

    xeno_load_process_filesystem(xenoReader);

    xeno_close_image(xenoReader);
}