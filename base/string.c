
#include "math.h"
#include "string.h"

void
StringCopy(string dst, string src, memory_index size)
{
    for (memory_index i = 0; i < size; i++)
    {
        *(dst++) = *(src++);
    }
}

inline memory_index
StringLength(string str)
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

string
StringNewLen(StringData* sd, void *str, memory_index size)
{
    sd->buf = (str) ? (uint8*)(str) : (uint8*)"\0";
    sd->size = ClampDown(size, StringLength((uint8*)str));
    /*sd->size = size;*/

    return(sd->buf);
}

string
StringNew(StringData* sd, void *str)
{
    string res = StringNewLen(sd, str, StringLength(str));

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
    // NOTE(liam): clamp to prevent overflow. +1 to include last character.
    memory_index endSizeClamped = ClampDown(last + 1, src.size);
    memory_index startSizeClamped = ClampDown(first, endSizeClamped);
    StringNewLen(dst, (src.buf + startSizeClamped), endSizeClamped - startSizeClamped);
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

void
StringPrint(StringData s)
{
    while (s.size--)
    {
        putc(*(s.buf++), stdout);
    }
    putc('\n', stdout);
}

