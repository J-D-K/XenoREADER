#include "XenoReader.h"

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

    printf("XenoGears Disc %i detected.\n",
           xeno_get_disc_number(xenoReader));

    xeno_close_image(xenoReader);
}