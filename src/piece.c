
#include "base/base.h"
#include "os/os.h"
#include <stdio.h>

// NOTE(liam): the piece ds here.

typedef struct Piece {
    uint8 type; // NOTE(liam): [0, 1]; 0 original, 1 add
    uint32 start; // offset to type buf.
    uint32 length; // relative to offset.
} Piece;

typedef struct AppendBuf {
    uint8 *buf;
    memory_index size;
    memory_index capacity;
} AppendBuf;

typedef struct PieceTable {
    StringData origBuf; // single immutable string

    AppendBuf addBuf;

    Piece *pieces;
    uint32 size;
    uint32 capacity;
} PieceTable;

#define BufIndex(ab, idx) (*((ab.buf) + (idx)))

StringData BufSlice(AppendBuf *ab, memory_index offset, memory_index size)
{
    StringData res = {0};

    memory_index actualSize = ClampDown(size, ab->size);
    StringNewLen(&res, (ab->buf + offset), actualSize);

    return res;
}

memory_index BufPush(Arena *arena, AppendBuf *ab, char *s)
{
    uint8 *s8 = (uint8 *)s;
    memory_index stringSize = StringLength(s8);
    memory_index res = stringSize;

    if (stringSize + ab->size > ab->capacity)
    {
        memory_index newCap = Max(ab->capacity * 2, Kilobytes(1));
        uint8 *newBuf = PushArray(arena, uint8, newCap);
        MemoryCopy(newBuf, ab->buf, ab->size);
        ab->buf = newBuf;
        ab->capacity = newCap;
    }

    uint8 *pos = (ab->buf + ab->size);
    while (stringSize--)
    {
        *(pos++) = *(s8++);
        ab->size++;
    }
    return res;
}


// TODO(liam): likely not working
memory_index PieceTableCheckIndex(PieceTable pt, memory_index offset)
{
    // NOTE(liam): given a logical offset,
    // return the idx position of the piece that contains
    // that offset, or 0 if the idx is at the start or end
    // of a piece.
    memory_index res = 1;

    uint32 logOffset = 0;
    for (uint32 i = 0; i < pt.size; i++)
    {
        Piece p = *(pt.pieces + i);


        logOffset += p.length;
    }
    printf("total logical offset: %d\n", logOffset);

    return res;
}

void PieceTableSet(Arena *arena,
                   PieceTable *pt,
                   memory_index length)
{
    // add the first piece in the table.
    if (pt->size + 1 > pt->capacity)
    {
        pt->capacity = 64;
        pt->pieces = PushArray(arena, Piece, pt->capacity);
    }

    Piece firstPiece = {0};

    firstPiece.start = 0;
    firstPiece.length = length;

    *(pt->pieces) = firstPiece;
    pt->size = 1;
}

void PieceTablePush(Arena *arena,
                    PieceTable *pt,
                    char *content,
                    memory_index offset)
{
    // NOTE(liam): immediately append new content to addBuf.
    memory_index contentSize = BufPush(arena, &pt->addBuf, content);

    // NOTE(liam): piece array could receive up to 2 new pieces
    // as a result of this insertion, so we're adjusting for the
    // worst case.
    if (pt->size + 2 > pt->capacity)
    {
        memory_index newCap = Max(pt->capacity * 2, Kilobytes(1));
        Piece *newPieces = PushArray(arena, Piece, newCap);
        MemoryCopy(newPieces, pt->pieces, pt->size);
        pt->pieces = newPieces;
        pt->capacity = newCap;
    }

    Piece midPiece = {
        .type = 1,
        .start = offset,
        .length = contentSize
    };

    // NOTE(liam): split old piece if necessary.
    Piece splitP[2];

    // NOTE(liam): check if offset overlaps with a piece.
    memory_index overlapIdx = PieceTableCheckIndex(*pt, offset);
    printf("this is the overlap idx: %lu.\n", overlapIdx);
    if (overlapIdx)
    {
        printf("im in here\n");
        // NOTE(liam): split the overlapped piece.
        Piece *p = pt->pieces + overlapIdx;

        splitP[0].start = p->start;
        splitP[0].length = offset;

        splitP[1].start = offset;
        splitP[1].length = p->length - offset;

        *p = splitP[0];
        *(pt->pieces + pt->size++) = midPiece;
        *(pt->pieces + pt->size++) = splitP[1];
    }
    else
    {
        // NOTE(liam): no need to split, but will need to modify
        *(pt->pieces + pt->size++) = midPiece;
    }



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
    uint8 buf[1024] = {0};
    uint32 origSize = 0;
    uint32 addSize = 0;
    for (uint32 i = 0; i < tb.size; i++)
    {
        Piece p = *(tb.pieces + i);
        StringData tmp = {0};

        if (p.type)
        {
            tmp = BufSlice(&tb.addBuf, addSize + p.start, addSize + p.length - 1);

            MemoryCopy(&tmp.buf, (buf + p.start), p.length);

            addSize += p.length;
        }
        else
        {
            StringSlice(&tmp, tb.origBuf, origSize + p.start, origSize + p.length - 1);

            MemoryCopy(&tmp.buf, (buf + p.start), p.length);
            origSize += p.length;
        }

    }

    printf("%s\n", buf);
}

void FileIndex(FILE *f, char *buf, int idx, int size)
{
    if (fseek(f, idx, SEEK_SET) != 0)
    {
        perror("Error seeking file");
    }

    size_t bytesRead = fread(buf, sizeof(*buf), size, f);
    if ((bytesRead != size) && (!feof(f)))
    {
        perror("Error reading file");
    }
}

int main(int argc, char **argv)
{
    Arena arena = {0};

    // management
    StringData fn[2];
    StringNew(&fn[0], "oc");
    StringNew(&fn[1], "tmp_weiss_buf");

    //FileDelete(&arena, fn[1]);
    StringData origBuf = FileRead(&arena, fn[0]);
    StringData addBuf = FileRead(&arena, fn[1]);

    //FILE *addBuf = fopen((char *)StringLiteral(fn[1]), "a+");
    PieceTable pt = {0};

    // NOTE(liam): populate buffers.
    pt.origBuf = origBuf;
    BufPush(&arena, &pt.addBuf, (char *)StringLiteral(addBuf));

    PieceTableSet(&arena, &pt, origBuf.size);
    PieceTablePush(&arena, &pt, "howdy\n", 4);

    PieceTablePrint(pt);

    ArenaFree(&arena);
    return 0;
};
