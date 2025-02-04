
#include "base.h"
#include "string.h"

int main(void)
{
    Arena local_arena = {0};

    // NOTE(liam): string is "zero" initialized.
    String s = "";

    StringNew(&local_arena, &s, "howdy!");

    /*StringPush(&local_arena, "", "boo hoo toodooloo!");*/
    /*StringPush(&local_arena, "", "cowabunga!");*/

    printf("%s\n", s);
    /*StringPrint(&s);*/

    ArenaFree(&local_arena);
    return 0;
}
