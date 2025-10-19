#include "XenoReader.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define PATH_BUFFER_SIZE 0xFF

static void extract_directory(XenoReader *reader, const XenoDir *dir, const char *target);

int main(int argc, const char *argv[])
{
    XenoReader *xenoReader = XenoReader_Open(argv[1]);
    if (!xenoReader)
    {
        printf("xenoReader is NULL!\n");
        return -1;
    }

    char outputDir[PATH_BUFFER_SIZE] = {0};
    snprintf(outputDir, PATH_BUFFER_SIZE, "./Xenogears_Disc_%i", XenoReader_GetDiscNumber(xenoReader));
    mkdir(outputDir);

    XenoDir *gearsRoot = XenoReader_GetRootDirectory(xenoReader);
    extract_directory(xenoReader, gearsRoot, outputDir);

    XenoReader_Close(xenoReader);
    printf("No segfault?\n");
}

static void extract_directory(XenoReader *reader, const XenoDir *dir, const char *target)
{
    static int dirCount = 0;

    char outputPath[PATH_BUFFER_SIZE] = {0};
    // 0 is always the root. This will make this less confusing IMO.
    if (dirCount == 0)
    {
        snprintf(outputPath, PATH_BUFFER_SIZE, "%s/DISC_ROOT", target);
        ++dirCount;
    }
    else {
        snprintf(outputPath, PATH_BUFFER_SIZE, "%s/DIR_%i", target, dirCount++);
    }

    // Ensure the end output path exists.
    mkdir(outputPath);

    // Need these to loop.
    const int subDirs = XenoDir_GetSubDirCount(dir);
    const int files   = XenoDir_GetFileCount(dir);

    for (int i = 0; i < subDirs; i++)
    {
        printf("Opening sub-directory %i...\n", i);
        const XenoDir *subDir = XenoDir_GetDirAt(dir, i);
        extract_directory(reader, subDir, outputPath);
    }

    for (int i = 0; i < files; i++)
    {
        // Grab the pointer to the file.
        const XenoFile *file = XenoDir_GetFileAt(dir, i);
        // This sector is basically padding.
        if (XenoFile_GetSector(file) == 0xFFFFFF) { break; }

        // Create this so we have it to print.
        char filePath[PATH_BUFFER_SIZE] = {0};
        snprintf(filePath, PATH_BUFFER_SIZE, "%s/FILE_%i.bin", outputPath, i);

        // Print a message so it looks like important things are happening when we're all just playing video games and
        // waiting to die.
        printf("Extracting file at sector 0x%0X to \"%s\"... ", XenoFile_GetSector(file), filePath);

        // Make reader read file to buffer to use.
        XenoBuffer *fileBuffer = XenoReader_ReadFile(reader, file);
        FILE *out              = fopen(filePath, "wb");
        if (!out)
        {
            printf("Error opening file for writing!\n");
            continue;
        }

        if (fwrite(fileBuffer->data, 1, fileBuffer->size, out) != fileBuffer->size)
        {
            printf("Error writing buffer contents to file!\n");
            XenoBuffer_Free(fileBuffer);
            fclose(out);
            continue;
        }

        fclose(out);
        XenoBuffer_Free(fileBuffer);
        printf("Finished!\n");
    }
}