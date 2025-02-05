
#include "base.h"

int main(void)
{
    Arena local_arena = {0};
    RandomSeries local_series = RandomSeed(9991);
    printf("Testing Random Seed:\n");
    printf("%f\n", RandomBilateral(&local_series));
    printf("%f\n", RandomBilateral(&local_series));
    printf("%f\n", RandomBilateral(&local_series));

    printf("New Char Lower of g: %c\n", CharLower('g'));
    printf("New Char Upper of p: %c\n", CharUpper('p'));

    StringData sdd = {0};
    StringNew(&sdd, "This is the test case!");

    printf("String Data alloc: %lu\n", sdd.size);

    StringData new_s = {0};

    StringSlice(&new_s, sdd, 2, 4); // [2, 4]
    StringPrint(new_s);
    StringPrefix(&new_s, sdd, 3);
    StringPrint(new_s);
    StringPostfix(&new_s, sdd, 3);
    StringPrint(new_s);
    StringSkipFront(&new_s, sdd, 3);
    StringPrint(new_s);
    StringSkipBack(&new_s, sdd, 3);
    StringPrint(new_s);

    putc('\n', stdout);

    StringList sll = {0};

    StringListPush(&local_arena, &sll, new_s);
    StringListPush(&local_arena, &sll, sdd);

    StringListPrint(sll);

    StringData sjoin = StringListJoin(&local_arena, &sll, null);


    printf("\nsize of new string: %llu\n", sjoin.size);
    StringPrint(sjoin);
    putc('\n', stdout);

    StringList nll = StringSplit(&local_arena, sjoin, " ", 2);

    StringListPrint(nll);
    putc('\n', stdout);

    printf("size of uint8 pointer + size_t: %zu\n", sizeof(memory_index) + sizeof(uint8*));

    ArenaFree(&local_arena);
    return 0;
}
