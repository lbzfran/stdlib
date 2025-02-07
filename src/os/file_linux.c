
#include <stdio.h>
#include "file.h"

String FileRead(Arena *arena, String filename)
{
    ArenaTemp tmp = ArenaScratchCreate(arena);

    FILE *file = fopen("some.txt", "r");
    if (!file)
    {
        // TODO(liam): handle err.
    }

    String res = {};

    //while (fgets())

    fclose(file);
    ArenaTempFree(arena);
}

bool32 FileWrite(String filename, StringList data)
{

}