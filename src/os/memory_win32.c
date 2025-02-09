
#include "memory.h"
#include <memoryapi.h>

void* AllocateMemory(memory_index size)
{
    void* res = VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    return(res);
}

void DeallocateMemory(void* memory, memory_index size_or_zero__)
{
    if (memory)
    {
        VirtualFree(memory, size_or_zero__, MEM_RELEASE);
    }
}
