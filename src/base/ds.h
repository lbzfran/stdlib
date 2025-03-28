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

#define ArrayU32Dump(A) do{\
    for (int i = 0; i < (A).size; i++){\
        printf("%u\n", A.V[i]);\
    };\
}while(0);

void ArrayU32Push(Arena *arena, ArrayUInt32 *a, uint32 x);

/***************************/

// Dictionaries


/*

list of keys
key data: uint8* (str)
hash data: uint16
value data: uint8* (str)

(key -> hash) 1:1 mapping
(hash -> value) 1:n mapping
linked list
*/


// typedef struct DictLinkedList {
//     uint32 hash;
//     uint32 value;
//     struct DictLinkedList* next;
// } DictLinkedList;
//
// typedef struct {
// 	uint32 *keys; // size/capacity relates to this
// 	memory_index size;
//     memory_index capacity;
//     memory_index minCapacity;
//
// 	DictLinkedList* values; // size of keys.
// } DictStruct;
//
// void DictAdd(DictStruct *, uint32, uint32);
// uint32 DictPop(DictStruct *, uint32);
// uint32 DictGet(DictStruct, uint32);
// void DictSetSize(DictStruct *dict, memory_index size);
// void DictClear(DictStruct *);
// void DictFree(void);

#endif //BASE_DS_H
