
#ifndef DYNARR_H
#define DYNARR_H

#include <stdlib.h>

#define dynarr(type) (type*)((int*)calloc(2, sizeof(int)) + 2)

#define dynarrPush(arrPtr, val) do { \
        void* arr = (void*)*arrPtr; \
        int* ptr = (int*)arr - 2; \
        ptr[0]++; \
        if(ptr[0] > ptr[1]) { \
            ptr[1] = ptr[1] == 0 ? 8 : 2 * ptr[1]; \
            size_t newSize = ptr[1] * sizeof(**arrPtr) + 2 * sizeof(int); \
            *arrPtr = (void*)realloc(ptr, newSize) + 2 * sizeof(int); \
        } \
        arr = (void*)*arrPtr; \
        ptr = (int*)arr - 2; \
        (*arrPtr)[ptr[0] - 1] = val; \
    } while(0);

#define dynarrFree(arrPtr) do { \
        int* ptr = (int*)*arrPtr - 2; \
        free(ptr); \
        *arrPtr = NULL; \
    } while(0);

#endif
