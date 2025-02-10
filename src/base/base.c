
#include "base.h"
#include "string.h"


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
    StringList nll = StringSplit(&local_arena, s_oldnews, ",");
    StringListPrintln(nll);


    uint8 utf8_test[4];

    /*StringDecode test1 = StringDecodeUTF8(utf8_test, ArrayCount(utf8_test));*/
    /*StringDecode test2 = StringDecodeUTF8(utf8_test + test1.length, ArrayCount(utf8_test) - test1.length);*/
    /*StringDecode test3 = StringDecodeUTF8(utf8_test + test2.length, ArrayCount(utf8_test) - test2.length);*/
    StringEncodeUTF8(utf8_test, 0x1F600);
    printf("UTF8 conversion: ");
    for (memory_index i = 0; i < 4; i++)
    {
        printf("encoded: %02x\n", utf8_test[i]);
        StringDecode dc = StringDecodeUTF8(utf8_test + i, 4 - i);
        printf("decoded: U+%x\n", dc.codepoint);
    }

    printf("CHATGPT conversions: ");
    uint32_t unicode_code_point = 0x1F600;

    // UTF-8 Encoding Example
    unsigned char utf8_encoded[4]; // UTF-8 encoding requires up to 4 bytes
    utf32_to_utf8(unicode_code_point, utf8_encoded);

    printf("UTF-8 Encoded (Hex): ");
    for (int i = 0; i < 4 && utf8_encoded[i] != 0; i++) {
        printf("%02X ", utf8_encoded[i]);
    }
    printf("\n");

    // UTF-8 Decoding Example
    uint32_t decoded_code_point = utf8_to_utf32(utf8_encoded);
    printf("Decoded Unicode Code Point: U+%X\n", decoded_code_point);


    /*printf("test1 codepoint value: %x\n", test1.codepoint);*/
    /*printf("test2 codepoint value: %x\n", test2.codepoint);*/
    /*printf("test3 codepoint value: %x\n", test3.codepoint);*/
    /*printf("test5 codepoint value: %x\n", test5.codepoint);*/
    /*printf("size encoded: %u.\n", test2);*/


    /*StringData sfmt = StringPushf(&local_arena, "%c %d", 46, 20);*/

    printf("size of uint8 pointer + size_t: %zu\n", sizeof(memory_index) + sizeof(uint8*));

    ArenaFree(&local_arena);
    return 0;
}
