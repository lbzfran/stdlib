
#include "base.h"

int main(void)
{
    Arena local_arena = {0};
    /*RandomSeries local_series = RandomSeed(9991);*/
    /*printf("Testing Random Seed:\n");*/
    /*printf("%f\n", RandomBilateral(&local_series));*/
    /*printf("%f\n", RandomBilateral(&local_series));*/
    /*printf("%f\n", RandomBilateral(&local_series));*/

    printf("New Char Lower of g: %c\n", CharLower('g'));
    printf("New Char Upper of p: %c\n", CharUpper('p'));

    StringData sdd = {0};
    StringNew(&sdd, "abcdefghijklmnopqrstuvwxyz");
    /*printf("String Data alloc: %lu\n", sdd.size);*/

    StringData new_s = {0};

    /*StringSlice(&new_s, sdd, 0, 3); // [2, 4]*/
    /*StringPrintn(new_s);*/
    /*StringSlice(&new_s, sdd, 3, sdd.size); // [2, 4]*/
    /*StringPrintn(new_s);*/
    /*StringPrefix(&new_s, sdd, 3);*/
    /*StringPrintn(new_s);*/
    /*StringPostfix(&new_s, sdd, 3);*/
    /*StringPrintn(new_s);*/
    /*StringSkipFront(&new_s, sdd, 3);*/
    /*StringPrintn(new_s);*/
    /*StringSkipBack(&new_s, sdd, 3);*/
    /*StringPrintn(new_s);*/


    StringList sll = {0};

    StringListPush(&local_arena, &sll, new_s);
    StringListPush(&local_arena, &sll, sdd);

    /*StringListPrintn(sll);*/
    /**/
    /*StringData sjoin = StringListJoin(&local_arena, &sll, null);*/
    /**/
    /*printf("size of new string: %llu\n", sjoin.size);*/
    /*StringPrintn(sjoin);*/

    StringData s_oldnews = {0};
    StringNew(&s_oldnews, "This, is, definitely, old, new.");
    /*StringData tmp = {0};*/
    /*StringSkipFront(&tmp, s_oldnews, 4);*/

    /*StringPrefix(&new_s, s_oldnews, 4);*/
    /*StringPrintn(new_s);*/
    /*StringSkipFront(&new_s, s_oldnews, 4);*/
    /*StringPrintn(new_s);*/

    /*StringPrintn(tmp);*/

    // TODO(liam): StringSplit not working as intended.
    StringList nll = StringSplit(&local_arena, s_oldnews, ",s");
    StringListPrintln(nll);

    /*StringData sfmt = StringPushf(&local_arena, "%c %d", 46, 20);*/

    printf("size of uint8 pointer + size_t: %zu\n", sizeof(memory_index) + sizeof(uint8*));

    ArenaFree(&local_arena);
    return 0;
}
