
#ifndef UTIL_H
#define UTIL_H

const char* readFile(const char* path);

#include <stdio.h>
#include <stdlib.h>

#define FATAL_ERROR(msg, ...) \
    do { \
        printf(__FILE__ "\nFATAL ERROR: " msg "\n", ##__VA_ARGS__); \
        exit(-1); \
    } while(0);

#define pair(a, b) struct {a f; b s;}

#endif
