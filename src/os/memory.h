#ifndef MEMORY_H
#define MEMORY_H

#include "base/def.h"
// NOTE(liam): Memory Allocation.

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
# define AllocateMemory(s) a_alloc((s))
# define DeallocateMemory(ptr, s) a_free((ptr))
#endif

// NOTE(liam): thread context
// TODO(liam): test this
/*#if !defined(SCRATCH_POOL_CAP)*/
/*# define SCRATCH_POOL_CAP 2*/
/*#endif*/
/*threadvar Arena *ScratchPool[SCRATCH_POOL_CAP] = 0;*/

#endif //MEMORY_H