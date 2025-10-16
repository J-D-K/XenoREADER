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

    printf("XenoGears Disc %i detected with %u total sectors.\n",
           xeno_get_disc_number(xenoReader),
           xeno_get_sector_count(xenoReader));

    Sector twentyFour;
    Sector twentyFive;

    xeno_seek_sector(24, xenoReader);
    xeno_read_sector(&twentyFour, xenoReader);
    xeno_read_sector(&twentyFive, xenoReader);

    FILE *output = fopen("TOC.txt", "w");

    fprintf(output, "Sector 24:\n");
    for (int i = 0, offset = 0; i < 332; i++, offset += 7)
    {
        uint32_t sector = 0;
        int32_t size    = 0;

        memcpy(&sector, &twentyFour.data[offset], 3);
        memcpy(&size, &twentyFour.data[offset + 3], sizeof(int32_t));
        if (sector == 0 || size == 0) { continue; }

        fprintf(output, "\tEntry[%i]:\n\t\tOffset?: %u\n\t\tSize: %i\n", i, sector, size);
    }

    fprintf(output, "Sector 25:\n");
    for (int i = 0, offset = 0; i < 292; i++, offset += 7)
    {
        uint32_t sector = 0;
        int32_t size    = 0;

        memcpy(&sector, &twentyFive.data[offset], 3);
        memcpy(&size, &twentyFive.data[offset + 3], sizeof(int32_t));
        if (sector == 0 || size == 0) { continue; }

        fprintf(output, "\tEntry[%i]:\n\t\tOffset?: %u\n\t\tSize: %i\n", i, sector, size);
    }
    fclose(output);

    xeno_close_image(xenoReader);
}