
#include "base/base.h"
#include "os/os.h"
#include <stdio.h>

// NOTE(liam): the piece ds here.

typedef struct Piece {
    uint8 type; // NOTE(liam): [0, 1]; 0 original, 1 add
    uint32 start; // offset to type buf.
    uint32 length; // relative to offset.
} Piece;

typedef struct PieceTable {
    /*StringData origBuf;*/
    uint32 origSize;

    FILE *addBuf;
    uint32 addSize;

    Piece *pieces;
    uint32 size;
    uint32 capacity;
} PieceTable;

void PieceTableAdd(
        Arena *arena,
        PieceTable *tb,
        uint8 *content,
        uint32 offset)
{
    // NOTE(liam): append new changes to addBuf.
    fprintf(tb->addBuf, (char *)content);

    // NOTE(liam): get the current position of addBuf.
    /*uint32 newAppendPos = (uint32)fseek(tb->addBuf, 0, SEEK_CUR);*/

    // NOTE(liam): calculate size of content.
    uint32 newAppendSize = (uint32)StringLength(content);

    uint32 logOffset = 0;

    Piece *oldP = (tb->pieces);
    uint32 max = 0;

    Piece *newPieces = PushArray(arena, Piece, 3);
}

/*void PieceTableDelete(PieceTable *tb, uint32 start_pos, uint32 length)*/
/*{*/
/*    Piece p = {*/
/*        .type = 0,*/
/*        .start = start_pos,*/
/*        .length = length*/
/*    };*/
/**/
/*    *(tb->pieces + tb->size++) = p;*/
/*}*/

/*uint8 PieceTableIndex(PieceTable tb, uint32 logOffset)*/
/*{*/
/*    // NOTE(liam): returns the character at the given logical offset*/
/*    // of the piece table.*/
/*    uint8 res = 0;*/
/*    uint32 j = 0;*/
/*    for (uint32 i = 0; i < tb.size; i++)*/
/*    {*/
/*        StringData res = {0};*/
/*        Piece p = *(tb.pieces + i);*/
/**/
/*        uint8 *localBuf = p.type ? tb.addBuf : tb.origBuf;*/
/**/
/*        StringPrefix(&res, (tb.addBuf + p.start), p.length);*/
/*        logOffset += p.length;*/
/**/
/*    }*/
/*    return res;*/
/*}*/

void PieceTablePrint(PieceTable tb)
{
    uint32 logOffset = 0;
    for (uint32 i = 0; i < tb.size; i++)
    {
        StringData res = {0};
        Piece p = *(tb.pieces + i);

        /*uint8 *localBuf = p.type ? tb.addBuf : tb.origBuf;*/

        /*StringPrefix(&res, (localBuf + p.start), p.length);*/
        logOffset += p.length;

        StringPrint(res);
    }
}

void PieceTableFree(PieceTable *tb)
{
    fclose(tb->addBuf);
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

    StringData origBuf = FileRead(&arena, fn);
    /*StringPrintn(origBuf);*/

    StringData p[3] = {0};

    StringSlice(&p[0], origBuf, 0, 10);
    StringNew(&p[1], "cowabunga");
    StringSlice(&p[2], origBuf, 10, 20);

    StringPrintn(p[0]);
    StringPrintn(p[1]);
    StringPrintn(p[2]);

    ArenaFree(&arena);
    return 0;
};
