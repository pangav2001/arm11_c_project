#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "headers/parse_file.h"
#include "headers/memory.h"

int32_t read_in_binary(char *fileName)
{
    FILE *inFile;
    int32_t fileSize;
    int8_t *buffer;

    inFile = fopen(fileName, "rb");

    if (inFile == NULL)
    {
        fputs("File error!", stderr);
        exit(1);
    }

    // get file size
    fseek(inFile, 0, SEEK_END);
    fileSize = ftell(inFile);
    rewind(inFile);

    if (fileSize > MEMORY_SIZE)
    {
        fputs("Larger file than expected!", stderr);
    }

    buffer = calloc(fileSize, sizeof(int8_t));
    assert(buffer);

    assert(fread(buffer, 1, fileSize, inFile) != 0);

    for (int i = 0; i < fileSize; i++)
    {
        store_memory(i, buffer[i]);
    }

    fclose(inFile);

    free(buffer);

    return fileSize;
}