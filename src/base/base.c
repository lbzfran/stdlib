
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
    StringList nll = StringSplit(&local_arena, s_oldnews, ",s");
    StringListPrintln(nll);

    printf("[Character Conversion]:\n");

    uint8 utf8_test[4];

    StringEncodeUTF8(utf8_test, 0x2603);
    printf("UTF8 conversion:");
    for (memory_index i = 0; i < 4 && utf8_test[i] != 0; i++)
    {
        printf(" %02x", utf8_test[i]);
    }
    putc('\n', stdout);
    uint32 decoded = 0;
    uint32 size = StringDecodeUTF8(&decoded, utf8_test);
    printf("decoded: U+%04x\n", decoded);
    printf("decoded size: %u\n", size);

    printf("UTF16 conversion:");
    uint16 utf16_test[2];
    StringEncodeUTF16(utf16_test, 0x1f60);
    for (memory_index i = 0; i < 2 && utf16_test[i] != 0; i++)
    {
        printf(" %02x", utf16_test[i]);
    }
    putc('\n', stdout);
    uint32 decoded2 = 0;
    uint32 size2 = StringDecodeUTF16(&decoded2, utf16_test);
    printf("decoded: U+%04x\n", decoded2);
    printf("decoded size: %u\n", size2);


    printf("[String utf8 Conversion]\n");

    StringData strpre = {0};
    StringNew(&strpre, "Howdy!");

    printf("strpre: ");
    StringPrintn(strpre);
    printf("size of strpre: %lu\n", strpre.size);

    String32Data strconv = StringConvert32(&local_arena, strpre);

    printf("hex value after conversion:");
    for (int i = 0; i < strconv.size && strconv.buf[i] != 0; i++)
    {
        printf(" %02x", *(strconv.buf + i));
    }
    putc('\n', stdout);
    printf("conversion buf size: %lu\n", strconv.size);

    StringData strpost = String32Convert(&local_arena, strconv);

    printf("strpost: ");
    StringPrintn(strpost);
    printf("size of strpost: %lu\n", strpost.size);

    printf("[String utf16 Conversion]\n");

    StringData str16pre = {0};
    StringNew(&str16pre, "Hola!");

    printf("str16pre: ");
    StringPrintn(str16pre);

    String16Data str16conv = StringConvert16(&local_arena, str16pre);

    printf("hex value after conversion:");
    for (int i = 0; i < str16conv.size && str16conv.buf[i] != 0; i++)
    {
        printf(" %02x", *(str16conv.buf + i));
    }
    putc('\n', stdout);
    printf("conversion buf size: %lu\n", str16conv.size);

    StringData str16post = String16Convert(&local_arena, str16conv);

    printf("str16post: ");
    StringPrintn(str16post);
    printf("size of strpost: %lu\n", str16post.size);



    /*StringData sfmt = StringPushf(&local_arena, "%c %d", 46, 20);*/

    //printf("size of uint8 pointer + size_t: %zu\n", sizeof(memory_index) + sizeof(uint8*));

    ArenaFree(&local_arena);
    return 0;
}
