
#include "memory.h"

#include <sys/mman.h>

/*typedef struct memory_block {*/
/*    struct memory_block* prev;*/
/*    struct memory_block* next;*/
/*    memory_index size;*/
/*    uint64 pad__[5];*/
/*} memory_block;*/
/**/
/*typedef struct memory_state {*/
/*    memory_block sentinel;*/
/*    pthread_mutex_t mutex;*/
/*} memory_state;*/
/*static memory_state memoryState;*/
/**/
/*void SetMemoryState()*/
/*{*/
/*    memory_state* state = &memoryState;*/
/*    memoryState.sentinel.prev = &state->sentinel;*/
/*    memoryState.sentinel.next = &state->sentinel;*/
/*}*/

void* AllocateMemory(memory_index size)
{
    /*Assert(sizeof(memory_block) == 64, "");*/
    void* res = /*(memory_block*)*/
        mmap(NULL, size,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (res == MAP_FAILED)
    {
        perror("Failed Allocation.");
        return NULL;
    }
    /*Assert(block, "");*/
    /*void* res = block + 1;*/
    /*memory_block* sentinel = &memoryState.sentinel;*/
    /**/
    /*block->next = sentinel;*/
    /*block->size = size;*/
    /**/
    /*pthread_mutex_lock(&memoryState.mutex);*/
    /*block->prev = sentinel->prev;*/
    /*block->prev->next = block;*/
    /*block->next->prev = block;*/
    /*pthread_mutex_unlock(&memoryState.mutex);*/
    return(res);
}

void DeallocateMemory(void* ptr, memory_index size)
{
    /*memory_block* block = ((memory_block*)ptr - 1);*/
    /*pthread_mutex_lock(&memoryState.mutex);*/
    /*block->prev->next = block->next;*/
    /*block->next->prev = block->prev;*/
    /*pthread_mutex_unlock(&memoryState.mutex);*/

    bool32 res = munmap(ptr, size);
    if (res == -1)
    {
        perror("Failed Deallocation.");
    }
}
