
#include "util.h"

#include <stdio.h>
#include <stdlib.h>

const char* readFile(const char* path) {
    FILE* file = fopen(path, "r");

    if(file == NULL) {
        return NULL;
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}
