
#include "base.h"

int main(void)
{
    Arena local_arena = {0};
    /*RandomSeries local_series = RandomSeed(9991);*/
    /**/
    /*printf("%f\n", RandomBilateral(&local_series));*/
    /*printf("%f\n", RandomBilateral(&local_series));*/
    /*printf("%f\n", RandomBilateral(&local_series));*/
    // NOTE(liam): string is "zero" initialized.
    String s = StringNew(&local_arena, "howdy!");

    printf("String: %s\n", s);

    ArenaFree(&local_arena);
    return 0;
}
