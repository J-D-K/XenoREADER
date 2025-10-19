#include "XenoReader.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

// This is the buffer size for paths.
#define PATH_BUFFER_SIZE 0xFF

// This is a recursive function to extract the contents of the disc image.
static void extract_directory(XenoReader *reader, const XenoDir *dir, const char *target, bool resetCount);

// This is so I don't need to change this when switching OS's
static inline void create_directory(const char *path)
{
#ifdef _WIN32
    mkdir(path);
#elif __linux__
    mkdir(path, 0777);
#endif
}

int main(int argc, const char *argv[])
{
    printf("--- XenoREADER Version 0.1 ---\n\n");
    if (argc <= 1)
    {
        printf("Usage: ./XenoREADER \"[path/to/XenogearsDisc1.bin]\" \"[path/to/XenogearsDisc2.bin]\"\n");
        return -1;
    }

    for (int i = 1; i < argc; i++)
    {
        printf("Opening \"%s\" and verifying image... ", argv[i]);
        XenoReader *xenoReader = XenoReader_Open(argv[i]);

        if (!xenoReader)
        {
            printf("File is not a valid Xenogears image!\n");
            continue;
        }

        printf("Xenogears Disc #%i detected.\n", XenoReader_GetDiscNumber(xenoReader));
        printf("Begin extracting contents...\n");

        XenoDir *root = XenoReader_GetRootDirectory(xenoReader);
        if (!root)
        {
            printf("Root directory is NULL!");
            continue;
        }

        // This is where we begin and put the root directory.
        char outputPath[PATH_BUFFER_SIZE] = {0};
        snprintf(outputPath, PATH_BUFFER_SIZE, "./Xenogears_Disc_%i", XenoReader_GetDiscNumber(xenoReader));
        create_directory(outputPath);

        extract_directory(xenoReader, root, outputPath, true);

        XenoReader_Close(xenoReader);
    }

    return 0;
}

static void extract_directory(XenoReader *reader, const XenoDir *dir, const char *target, bool resetCount)
{
    static int dirCount = 0;
    if (resetCount) { dirCount = 0; }

    char outputPath[PATH_BUFFER_SIZE] = {0};
    // 0 is always the root. This will make this less confusing IMO.
    if (dirCount == 0)
    {
        snprintf(outputPath, PATH_BUFFER_SIZE, "%s/DISC_ROOT", target);
        ++dirCount;
    }
    else {
        snprintf(outputPath, PATH_BUFFER_SIZE, "%s/DIR_%04d", target, dirCount++);
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
        extract_directory(reader, subDir, outputPath, false);
    }

    for (int i = 0; i < files; i++)
    {
        // Grab the pointer to the file.
        const XenoFile *file = XenoDir_GetFileAt(dir, i);
        // This sector is basically padding.
        if (XenoFile_GetSector(file) == 0xFFFFFF) { break; }

        // Create this so we have it to print.
        char filePath[PATH_BUFFER_SIZE] = {0};
        snprintf(filePath, PATH_BUFFER_SIZE, "%s/FILE_%04d.bin", outputPath, i + 1);

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