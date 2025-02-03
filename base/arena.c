#include "arena.h"

void
ArenaFillZero(memory_index size, void *ptr) // effectively memcpy
{
    uint8* byte = (uint8*) ptr;
    while (size--) {
        *byte++ = 0;
    }
}

void
ArenaSetMinimumBlockSize(Arena* arena, memory_index minimumBlockSize)
{
    arena->minimumBlockSize = minimumBlockSize;
}

memory_index
ArenaGetAlignmentOffset(Arena* arena, memory_index alignment)
{
    memory_index alignmentOffset = 0;

    memory_index resPointer = (memory_index)arena->base + arena->pos;
    memory_index alignmentMask = alignment - 1;
    if (resPointer & alignmentMask)
    {
        alignmentOffset = alignment - (resPointer & alignmentMask);
    }

    return(alignmentOffset);
}

memory_index
ArenaGetRemainingSize(Arena* arena, memory_index alignment)
{
    memory_index res = arena->size - (arena->pos + ArenaGetAlignmentOffset(arena, alignment));
    return(res);
}

memory_index
ArenaGetEffectiveSize(Arena* arena, memory_index sizeInit, memory_index alignment)
{
    memory_index size = sizeInit;

    memory_index alignmentOffset = ArenaGetAlignmentOffset(arena, alignment);
    size += alignmentOffset;

    return(size);
}

bool32
ArenaCanStoreSize(Arena* arena, memory_index sizeInit, memory_index alignment)
{
    if (!alignment) alignment = DEFAULT_ALIGNMENT;

    memory_index size = ArenaGetEffectiveSize(arena, sizeInit, alignment);
    bool32 res = (arena->pos + size <= arena->size);

    return(res);
}

ArenaFooter*
GetFooter(Arena* arena)
{
    ArenaFooter *res = (ArenaFooter*)(arena->base + arena->size);

    return(res);
}

void*
ArenaPush(Arena* arena, memory_index sizeInit, memory_index alignment)
{
    if (!alignment) alignment = DEFAULT_ALIGNMENT;

    //NOTE(liam): rounds allocation up to set align properly.
    memory_index size = ArenaGetEffectiveSize(arena, sizeInit, alignment);

    /*Assert(arena->pos + size < arena->size, "requested alloc size exceeds arena size.")*/
    if ((arena->pos + size) > arena->size)
    {
        // NOTE(liam): if min block size was never set, set it.
        if (!arena->minimumBlockSize)
        {
            // TODO(liam): tune block sizing
            arena->minimumBlockSize = DEFAULT_BLOCKSIZE; // 1024 * 1024
        }

        ArenaFooter save = {0};
        save.base = arena->base;
        save.size = arena->size;
        save.pos = arena->pos;

        // NOTE(liam): base should automatically align after allocating again.
        size = sizeInit;
        memory_index blockSize = Max(size + sizeof(struct memory_arena_footer), arena->minimumBlockSize);
        arena->size = blockSize - sizeof(struct memory_arena_footer);
        arena->base = (uint8*)AllocateMemory(blockSize);
        arena->pos = 0;
        arena->blockCount++;

        ArenaFooter* footer = GetFooter(arena);
        *footer = save;
    }
    Assert((arena->pos + size) <= arena->size, "new allocation of dynamic arena somehow failed...");

    memory_index alignmentOffset = ArenaGetAlignmentOffset(arena, alignment);
    void* res = (void*)(arena->base + arena->pos - alignmentOffset);
    arena->pos += size;

    Assert(size >= sizeInit, "requested alloc exceeds arena size after alignment.");

    return(res);
}

void*
ArenaCopy(memory_index size, void* src, void* dst)
{
    uint8* srcPos = (uint8*)src;
    uint8* dstPos = (uint8*)dst;
    while (size--)
    {
        *dstPos++ = *srcPos++;
    }
    return(dst);
}

void
SubArena(Arena* subArena, Arena* arena, memory_index size, memory_index alignment)
{
    if (!alignment) alignment = DEFAULT_ALIGNMENT;

    subArena->size = size;
    subArena->base = (uint8*)PushSizeAlign(arena, size, alignment);
    subArena->pos = 0;
    subArena->tempCount = 0;
}


void
ArenaPop(Arena* arena, memory_index size)
{
    if ((arena->size - size) > 0)
    {
        arena->size -= size;
    }
    else { arena->size = 0; }
}

// NOTE(liam): effectively resets the Arena.
void
ArenaClear(Arena *arena)
{
    while (arena->blockCount)
    {
        ArenaFreeCurrentBlock(arena);
    }
}

// NOTE(liam): temporary memory.
ArenaTemp
ArenaTempBegin(Arena *arena)
{
    ArenaTemp res;

    res.arena = arena;
    res.base = arena->base;
    res.pos = arena->pos;

    arena->tempCount++;

    return(res);
}

void
ArenaFreeCurrentBlock(Arena* arena)
{
    void* freedBlock = arena->base;
    memory_index freedBlockSize = arena->size;

    ArenaFooter* footer = GetFooter(arena);

    arena->base = footer->base;
    arena->size = footer->size;
    arena->pos  = footer->pos;

    DeallocateMemory(freedBlock, freedBlockSize);

    arena->blockCount--;
}

void
ArenaTempEnd(ArenaTemp temp)
{
    Arena* arena = temp.arena;

    while(arena->base != temp.base)
    {
        ArenaFreeCurrentBlock(arena);
    }

    Assert(arena->pos >= temp.pos, "Arena position is less than temporary memory's position. Likely user-coded error.");
    arena->pos = temp.pos;

    Assert(arena->tempCount > 0, "Attempt to decrement Arena's temporary memory count when it is already 0.");
    arena->tempCount--;
}

// NOTE(liam): should call after finishing temp use.
// need to make sure all temps are accounted for before
// resuming allocations.
void
ArenaTempCheck(Arena* arena)
{
    Assert(arena->tempCount == 0, "")
}

ArenaTemp
ArenaScratchCreate(Arena* arena)
{
    //TODO(liam): replace assertion.
    Assert(arena->pos + sizeof(ArenaTemp) <= arena->size, "requested temp alloc exceeds arena size.");

    ArenaTemp temp = ArenaTempBegin(arena);
    return temp;
}
