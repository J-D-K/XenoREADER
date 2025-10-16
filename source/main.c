#include "XenoReader.h"

#include <stdio.h>

int main(int argc, const char *argv[])
{
    XenoReader *xenoReader = xeno_open_image(argv[1]);
    if (!xenoReader)
    {
        printf("xenoReader is NULL!\n");
        return -1;
    }

    printf("XenoGears Disc %i detected with %u total sectors.\n",
           xeno_get_disc_number(xenoReader),
           xeno_get_sector_count(xenoReader));

    xeno_close_image(xenoReader);
}