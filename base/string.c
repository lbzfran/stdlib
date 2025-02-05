
#include "string.h"

void
StringCopy(void *dst, void *src, memory_index n)
{
    String s1 = (String)dst;
    String s2 = (String)src;
    for (memory_index i = 0; i < n; i++)
    {
        *(s1++) = *(s2++);
    }
}

inline memory_index
StringLength(String str)
{
    memory_index size = 0;

    while (*(str + size) != '\0') { size++; }

    return(size);
}

inline uint8
CharLower(uint8 c)
{
    if ('A' <= c && c <= 'Z') { c = c - ('A' - 'a'); }
    return(c);
}

inline uint8
CharUpper(uint8 c)
{
    if ('a' <= c && c <= 'z') { c = c - ('a' - 'A'); }
    return(c);
}

/*---*/

StringData *
StringDataAlloc(Arena *arena, memory_index length)
{
    StringData *sData = (StringData*)PushSize(arena, sizeof(struct StringData) + length + 1);
    sData->capacity = length + 1;
    sData->size = length;
    return(sData);
}

String
StringNewLen(Arena *arena, void *str, memory_index size)
{
    StringData *sData = NULL;

    if (str)
    {
        sData = StringDataAlloc(arena, size);
        StringCopy(&sData->buf, str, size);
        *(sData->buf + size) = '\0';
    }
    else
    {
        sData = StringDataAlloc(arena, size);
        *sData->buf = '\0';
    }
    return(sData->buf);
}

String
StringNew(Arena *arena, void *str)
{
    String res = StringNewLen(arena, str, StringLength(str));

    return(res);
}

StringData *
StringGetData(String s)
{
	StringData *res = (StringData*)&s[-sizeof(struct StringData)];

	return(res);
}

String
StringSlice(Arena *arena, String s, memory_index first, memory_index last)
{
    String res = StringNewLen(arena, (s + first), last);

    return(res);
}

String
StringPrefix(Arena *arena, String s, memory_index last)
{
    String res = StringSlice(arena, s, 0, last);

    return(res);
}

String
StringPostfix(Arena *arena, String s, memory_index first)
{
    // TODO(liam): check this gets correct end size.
    StringData *sData = StringGetData(s);
    String res = StringSlice(arena, s, first, sData->size);

    return(res);
}


