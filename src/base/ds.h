#ifndef BASE_DS_H
#define BASE_DS_H

#include "def.h"
#include "arena.h"
// typical generic constructs here.

// Generic Dynamic Array
typedef struct {
    uint32 *V;
    uint32 size;
    uint32 capacity;
} ArrayUInt32, ArrayU32;

void ArrayU32Push(Arena *arena, ArrayUInt32 *a, uint32 x);

#define ArrayU32Dump(A) do{\
    for (int i = 0; i < (A).size; i++){\
        printf("%u\n", A.V[i]);\
    };\
}while(0);

#endif //BASE_DS_H
