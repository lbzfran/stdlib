
#include "base.h"
#include "string.h"

int main(void)
{
    Arena local_arena = {0};

    String s = {0};

    StringPush(&local_arena, &s, "howdy!");

    StringPush(&local_arena, &s, "boo hoo toodooloo!");
    StringPush(&local_arena, &s, "cowabunga!");

    StringPrint(&s);

    ArenaFree(&local_arena);
    return 0;
}
