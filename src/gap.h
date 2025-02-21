
#include "base/base.h"
#include "os/os.h"

#define GAP_INITIAL_SIZE 512

typedef struct GapBuf {
    uint32 left;
    uint32 right;

    char *buf;
    memory_index capacity;
} GapBuf;

void GapGrow(Arena *arena, GapBuf *gb);
void GapInsert(Arena *arena, GapBuf *gb, char c);
bool32 GapLoad(Arena* arena, GapBuf *gb, char *filename);
bool32 GapDelete(GapBuf *gb);
/*void GapMove(GapBuf *gb, int pos);*/
bool32 GapMoveLeft(GapBuf *gb);
bool32 GapMoveRight(GapBuf *gb);
void GapPrint(GapBuf gb);
