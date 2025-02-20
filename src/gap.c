
#include "base/base.h"
#include "os/os.h"
#include <stdio.h>


typedef struct GapBuf {
    uint32 left;
    uint32 right;

    char *buf;
    memory_index capacity;
} GapBuf;

void GapGrow(Arena *arena, GapBuf *gb)
{
    memory_index newCap = Max(gb->capacity * 2, 64);
    memory_index K = newCap - gb->capacity;
    printf("Growing buffer! size from %lu to %lu\n", gb->capacity, newCap);
    char *newBuf = PushArray(arena, char, newCap);
    ArenaFillZero(newCap, newBuf);

    for (memory_index i = 0; i < gb->left; i++)
    {
        *(newBuf + i) = *(gb->buf + i);
    }
    for (memory_index i = gb->right; i < gb->capacity; i++)
    {
        *(newBuf + i + K) = *(gb->buf + i);
    }
    /*MemoryCopy(newBuf, gb->buf, gb->left);*/
    /*MemoryCopy(newBuf + K, gb->buf + gb->right, gb->capacity - gb->right);*/

    gb->right = gb->right + K;
    gb->buf = newBuf;
    gb->capacity = newCap;
}


void GapInsert(Arena *arena, GapBuf *gb, char c)
{
    if (gb->left == gb->right)
    {
        GapGrow(arena, gb);
    }
    gb->buf[gb->left] = c;
    gb->left = gb->left + 1;
}

void GapDelete(GapBuf *gb)
{
    if (gb->left != 0)
    {
        gb->left--;
    }
}

void GapShiftLeft(GapBuf *gb)
{
    if (gb->left != 0)
    {
        gb->left--;
        gb->right--;
        gb->buf[gb->right] = gb->buf[gb->left];
    }
}

void GapShiftRight(GapBuf *gb)
{
    if (gb->right != 0)
    {
        gb->left++;
        gb->right++;
        gb->buf[gb->left] = gb->buf[gb->right];
    }
}

void GapBufPrint(GapBuf gb)
{
    for (int i = 0; i < gb.left; i++)
    {
        char c = *(gb.buf + i);
        {
            putc(c, stdout);
        }
    }
    for (int i = gb.right; i < gb.capacity; i++)
    {
        char c = *(gb.buf + i);
        {
            putc(c, stdout);
        }
    }
    putc('\n', stdout);
}

int main(void)
{
    Arena arena = {0};
    GapBuf b = {0};

    GapInsert(&arena, &b, 'a');
    GapBufPrint(b);
    GapInsert(&arena, &b, 'b');
    GapBufPrint(b);
    GapInsert(&arena, &b, 'c');
    GapBufPrint(b);

    GapShiftLeft(&b);
    GapShiftLeft(&b);

    GapInsert(&arena, &b, 'd');
    GapBufPrint(b);
    GapInsert(&arena, &b, 'e');
    GapBufPrint(b);

    GapShiftRight(&b);
    GapDelete(&b);

    GapBufPrint(b);
    ArenaFree(&arena);
    return 0;
}
