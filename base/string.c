
#include "math.h"
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

String
StringNewLen(StringData* sd, void *str, memory_index size)
{
    sd->buf = (str) ? (uint8*)(str) : (uint8*)"\0";
    sd->size = ClampDown(size, StringLength((uint8*)str));
    /*sd->size = size;*/

    return(sd->buf);
}

String
StringNew(StringData* sd, void *str)
{
    String res = StringNewLen(sd, str, StringLength(str));

    return(res);
}

// NOTE(liam): ability to get the struct of an existing 'String' datatype.
// passing arguments that was not explicitly initialized as 'String' will
// definitely segfault, and is not within intended use for this structure.
/*StringData **/
/*StringGetData(String s)*/
/*{*/
/*	StringData *res = (StringData*)&s[-sizeof(struct StringData)];*/
/**/
/*	return(res);*/
/*}*/

void
StringSlice(StringData *dst, StringData src, memory_index first, memory_index last)
{
    // NOTE(liam): clamp to prevent overflow.
    memory_index endSizeClamped = ClampDown(last, src.size);
    memory_index startSizeClamped = ClampDown(first, endSizeClamped);
    StringNewLen(dst, (src.buf + endSizeClamped), endSizeClamped - startSizeClamped);
}

// Note(liam): small visualizations. [...] = string, 0 = keep, x = remove.
// Note(liam): [00|xxxxx]
void
StringPrefix(StringData *dst, StringData src, memory_index size)
{
    memory_index sizeClamped = ClampDown(size, src.size);
    StringNewLen(dst, src.buf, sizeClamped);
}

// Note(liam): [xxxxx|00]
void
StringPostfix(StringData *dst, StringData src, memory_index size)
{
    // NOTE(liam): forces clamp down using infinity.
    memory_index sizeClamped = ClampDown(size, src.size);
    memory_index startPos = src.size - sizeClamped;
    StringNewLen(dst, (src.buf + startPos), sizeClamped);
}

// Note(liam): [xx|00000]
void
StringSkipFront(StringData *dst, StringData src, memory_index count)
{
    memory_index countClamped = ClampDown(count, src.size);
    memory_index startPos = src.size - countClamped;
    StringNewLen(dst, (src.buf + startPos), countClamped);
}

// Note(liam): [00000|xx]
void
StringSkipBack(StringData *dst, StringData src, memory_index count)
{
    memory_index countClamped = ClampDown(count, src.size);
    StringNewLen(dst, src.buf, countClamped);
}


