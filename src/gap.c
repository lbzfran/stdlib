
#include "base/base.h"
#include "os/os.h"
#include <stdio.h>

#define GAP_INITIAL_SIZE 512

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
    gb->buf[gb->left++] = c;
}

bool32 GapLoad(Arena* arena, GapBuf *gb, char *filename)
{
    bool32 res = false;
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("");
    }
    else
    {
        fseek(file, 0, SEEK_END);
        memory_index fileSize = (memory_index)ftell(file);
        fseek(file, 0, SEEK_SET);

        gb->capacity = fileSize + GAP_INITIAL_SIZE;
        gb->buf = PushArray(arena, char, gb->capacity);

        gb->left = fileSize;
        gb->right = fileSize + GAP_INITIAL_SIZE;

        fread(gb->buf, 1, fileSize, file);

        fclose(file);
    }

    return res;
}

void GapDelete(GapBuf *gb)
{
    if (gb->left != 0)
    {
        --gb->left;
    }
}

void GapShiftLeft(GapBuf *gb)
{
    if (gb->left != 0)
    {
        gb->buf[--gb->right] = gb->buf[--gb->left];
    }
}

void GapShiftRight(GapBuf *gb)
{
    if (gb->right != 0)
    {
        gb->buf[++gb->left] = gb->buf[++gb->right];
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

    GapLoad(&arena, &b, "./src/piece.txt");

    for (int i = 0; i < 50; i++)
    {
        GapShiftLeft(&b);
    }

    GapInsert(&arena, &b, 'A');
    GapInsert(&arena, &b, 'B');
    GapInsert(&arena, &b, 'C');

    for (int i = 0; i < 20; i++)
    {
        GapShiftRight(&b);
    }
    for (int i = 0; i < 10; i++)
    {
        GapDelete(&b);
    }

    GapBufPrint(b);
    ArenaFree(&arena);
    return 0;
}
