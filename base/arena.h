/*
 * ---------------
 * Liam Bagabag
 * Version: c2.4
 * Requires: platform.h
 * ---------------
 */
#ifndef ARENA_H
#define ARENA_H

// NOTE(liam): define 'ARENA_USEMALLOC' if you want to allocate memory yourself.
// afterwards, you have to define both 'a_alloc' and 'a_free', or
// it will fallback to using the std lib malloc implementation.
// NOTE(liam): when not defining 'ARENA_USEMALLOC', you will need to compile
// the respective c file (arena_memory_<linux/win32>.c) with your project,
// or define the respective functions yourself.
/*#define ARENA_USEMALLOC*/

#include "memory.h"

#include <stdalign.h>
#define DEFAULT_ALIGNMENT sizeof(void*)
#define DEFAULT_BLOCKSIZE Kilobytes(16)

typedef struct memory_arena_footer {
    uint8* base;
    memory_index size;
    memory_index pos;
    memory_index padding;
} ArenaFooter;

typedef struct memory_arena {
    uint8* base;
    memory_index size;
    memory_index pos; // aka used memory idx

    memory_index minimumBlockSize;

    uint32 blockCount;
    uint32 tempCount;
} Arena;

typedef struct memory_arena_temp {
    Arena* arena;
    uint8* base;
    memory_index pos;
    memory_index padding;
} ArenaTemp;

void* ArenaPush(Arena*, memory_index, memory_index);
void ArenaPop(Arena* arena, memory_index size);
void* ArenaCopy(memory_index, void*, void*);
ArenaFooter* GetFooter(Arena* arena);

memory_index ArenaGetEffectiveSize(Arena* arena, memory_index sizeInit, memory_index alignment);
memory_index ArenaGetAlignmentOffset(Arena* arena, memory_index alignment);
memory_index ArenaGetRemainingSize(Arena* arena, memory_index alignment);

void ArenaFreeCurrentBlock(Arena* arena);

// NOTE(liam): auto-aligned Push Instructions.
#define PushArray(arena, t, c) (t*)ArenaPush((arena),sizeof(t)*(c), alignof(t))
#define PushStruct(arena, t) PushArray(arena, t, 1)
#define PushSize(arena, s) ArenaPush((arena), (s), alignof(s))
#define PushCopy(arena, s, src) ArenaCopy(s, src, ArenaPush(arena, s, alignof(s)))

// NOTE(liam): Set Alignment Manually.
#define PushArrayAlign(arena, t, c, ...) (t*)ArenaPush((arena),sizeof(t)*(c), ## __VA_ARGS__)
#define PushStructAlign(arena, t, ...) PushArray(arena, t, ## __VA_ARGS__)
#define PushSizeAlign(arena, s, ...) ArenaPush((arena), (s), ## __VA_ARGS__)
#define PushCopyAlign(arena, s, src, ...) ArenaCopy(s, src, ArenaPush(arena, s, ## __VA_ARGS__))
void ArenaFillZero(memory_index size, void *ptr);

uint64 ArenaGetPos(Arena*);

void ArenaSetMinimumBlockSize(Arena* arena, memory_index minimumBlockSize);
void ArenaSetPos(Arena*, memory_index);
void ArenaClear(Arena*);
#define ArenaFree(arena) ArenaClear(arena);

void SubArena(Arena* subArena, Arena* arena, memory_index size, memory_index alignment);

ArenaTemp ArenaTempBegin(Arena*);
void ArenaTempEnd(ArenaTemp);
void ArenaTempCheck(Arena*);

ArenaTemp ArenaScratchCreate(Arena*);
#define ArenaScratchFree(t) ArenaTempEnd(t)

#define ZeroStruct(in) ArenaFillZero(sizeof(in), &(in))
#define ZeroArray(n, ptr) ArenaFillZero((n)*sizeof((ptr)[0]), (ptr))


#endif //ARENA_H
