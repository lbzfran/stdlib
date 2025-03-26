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
} uint32Array, u32Array;

void uint32ArrayAppend(Arena *arena, uint32Array *a, uint32 x);

#endif //BASE_DS_H
