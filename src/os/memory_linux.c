
#include "memory.h"
#include <sys/mman.h>

void* AllocateMemory(memory_index size)
{
    void* res = /*(memory_block*)*/
        mmap(NULL, size,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (res == MAP_FAILED)
    {
        perror("Failed Allocation.");
        return NULL;
    }
    return(res);
}

void DeallocateMemory(void* ptr, memory_index size)
{
    bool32 res = munmap(ptr, size);
    if (res == -1)
    {
        perror("Failed Deallocation.");
    }
}
