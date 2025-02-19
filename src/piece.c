
#include "base/base.h"
#include "os/os.h"

// NOTE(liam): the piece ds here.

typedef struct Piece {
    uint8 type; // NOTE(liam): [0, 1]; 0 original, 1 add
    uint32 start; // offset to type buf.
    uint32 length; // relative to offset.
} Piece;

typedef struct PieceTable {
    StringData origBuf;
    uint32 origSize;

    FILE *addBuf;
    uint32 addSize;

    Piece *pieces;
    uint32 size;
    uint32 capacity;
} PieceTable;
/*
 * example:
 *  t: O, start: 0, len: 10
 *  t: 1, start: 0, len: 5
 *  t: 0, start: 10, len: 22
 *
 */

void PieceTableAdd(Arena *arena, PieceTable *tb, uint8 *content, uint32 offset)
{
    // NOTE(liam): append new changes to addBuf.
    fprintf(tb->addBuf, content);

    // NOTE(liam): get the current position of addBuf.
    uint32 newAppendPos = (uint32)fseek(tb->addBuf, 0, SEEK_CUR);

    // NOTE(liam): calculate size of content.
    uint32 newAppendSize = (uint32)StringLength(content);

    uint32 logOffset = 0;

    Piece *oldP = (tb->pieces);
    uint32 max = 0;
    while (1)
    {
        // NOTE(liam): find the piece containing the given offset.

        oldP++;
    }

    Piece PreP = {
        .type = 0,
        .start = oldP.start,
    };

    // NOTE(liam): find piece to modify.
    Piece p = {
        .type = 1,
        .start = start_pos,
        .length = size
    };

    Piece PostP = {
        .type = 0,
        .start = (PreP.start + PreP.length),
        .length = ...
    };

    *(tb->pieces + size++) = p;
    *(tb->pieces + size++) = PostP;
}

void PieceTableDelete(PieceTable *tb, uint32 start_pos, uint32 length)
{
    Piece p = {
        .type = 0,
        .start = start_pos,
        .length = length
    };

    *(tb->pieces + tb->size++) = p;
}

uint8 PieceTableIndex(PieceTable tb, uint32 idx)
{
    uint8 res = 0;
    uint32 j = 0;
    while (1)
    {
        Piece p = *(tb->pieces + j);
        if (idx - p.start < p.length)
        {
            idx = idx - p.start;
            res = *(p.buf + idx);
            break;
        }
        j++;
    }
    return res;
}

void PieceTablePrint(PieceTable tb)
{
    uint32 logOffset = 0;
    for (uint32 i = 0; i < tb.size; i++)
    {
        StringData res = {0};
        Piece p = *(tb.pieces + i);

        uint8 *localBuf = p.type ? tb.addBuf : tb.origBuf;

        StringPrefix(&res, (tb.addBuf + p.start), p.length);
        logOffset += p.length;

        StringPrint(res);
    }
}

int main(int argc, char **argv)
{
    Arena arena = {0};

    if (argc < 2)
    {
        return 1;
    }
    StringData fn = {0};
    StringNew(&fn, argv[1]);

    origBuf = FileRead(&arena, fn);
    if (!origBuf.size)
    {
        return 1;
    }

    // NOTE(liam): not cross platform code, yet.
    FILE *addPtr = fopen("tmp_weiss_buf", "a+");

    PieceTable tb = {0};
    PieceTableAppend(origBuf.buf, 0, origBuf.size);


    ArenaFree(&arena);
    return 0;
};
