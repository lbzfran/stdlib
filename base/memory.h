#ifndef MEMORY_H
#define MEMORY_H

#include "defines.h"
// NOTE(liam): Memory Allocation.
// TODO(liam): figure out how to further abstract this;
// see handmade hero: find platform layer, platform_allocate_memory, etc.
/*#define PLATFORM_ALLOCATE_MEMORY(fn) fn()*/
/*struct Platform {*/
/*    void* (*AllocateMemory)(memory_index);*/
/*    void (*DeallocateMemory)(void*, memory_index);*/
/*};*/

#ifndef ARENA_USEMALLOC
// NOTE(liam): likely do not modify this.
void* AllocateMemory(memory_index);
void DeallocateMemory(void*, memory_index);
#else
# if !defined(a_alloc) || !defined(a_free)
#  include <stdlib.h>
#  define a_alloc malloc
#  define a_free free
# endif
#
# define AllocateMemory(s) a_alloc((s))
# define DeallocateMemory(ptr, s) a_free((ptr))
#endif

#endif //MEMORY_H

