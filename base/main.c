
#include "base.h"

int main(void)
{
    Arena local_arena = {0};
    /*RandomSeries local_series = RandomSeed(9991);*/
    /*printf("%f\n", RandomBilateral(&local_series));*/
    /*printf("%f\n", RandomBilateral(&local_series));*/
    /*printf("%f\n", RandomBilateral(&local_series));*/

    /*String s = StringNew(&local_arena, "howdy!");*/
    StringData sdd = {0};
    StringNew(&sdd, "howdy!");
    // String s = sdd->buf;
    printf("String Data alloc: %lu\n", sdd.size);

    StringData new_s = {0};

    StringSlice(&new_s, sdd, 1, 3);
    StringPrefix(&new_s, sdd, 3);
    StringPostfix(&new_s, sdd, 3);
    StringSkipFront(&new_s, sdd, 3);
    StringSkipBack(&new_s, sdd, 3);

    /*printf("New String Slice: %s\n", new_s.buf);*/

    printf("New Char Lower of g: %c\n", CharLower('g'));
    printf("New Char Upper of p: %c\n", CharUpper('p'));

    printf("size of uint8 pointer + size_t: %zu\n", sizeof(memory_index) + sizeof(uint8*));

    ArenaFree(&local_arena);
    return 0;
}
