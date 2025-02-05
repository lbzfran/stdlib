
#include "base.h"

int main(void)
{
    Arena local_arena = {0};
    /*RandomSeries local_series = RandomSeed(9991);*/
    /**/
    /*printf("%f\n", RandomBilateral(&local_series));*/
    /*printf("%f\n", RandomBilateral(&local_series));*/
    /*printf("%f\n", RandomBilateral(&local_series));*/

    String s = StringNew(&local_arena, "howdy!");

    printf("String: %s\n", s);

    StringData sd = *StringGetData(s);
    printf("String Data: %lu of %lu\n", sd.size, sd.capacity);

    String new_s = StringPostfix(&local_arena, s, 3);

    printf("New String Slice: %s\n", new_s);

    printf("New Char Lower of S: %c\n", CharLower('S'));
    printf("New Char Upper of s: %c\n", CharUpper('s'));

    ArenaFree(&local_arena);
    return 0;
}
