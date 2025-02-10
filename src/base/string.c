
#include "math.h"
#include "string.h"
#include <stdarg.h>

void
MemoryCopy(void *dst, void *src, memory_index size)
{
    uint8* cd = (uint8*)dst;
    uint8* cs = (uint8*)src;
    for (memory_index i = 0; i < size; i++)
    {
        *(cd++) = *(cs++);
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

string
StringNewRange(StringData *sd, string first, string last_optional)
{
    sd->buf = first;
    sd->size = (memory_index)(last_optional - first);
    return(sd->buf);
}

// NOTE(liam): ability to get the struct of an existing 'String' datatype.
// passing arguments that was not explicitly initialized as 'String' will
// definitely segfault, and is not within intended use for this structure.
/*
StringData *
StringGetData(String s)
{
	StringData *res = (StringData*)&s[-sizeof(struct StringData)];

	return(res);
}
*/

void
StringSlice(StringData *dst, StringData src, memory_index first, memory_index last)
{
    // NOTE(liam): clamp to prevent overflow. +1 to include last character.
    memory_index endSizeClamped = ClampDown(last + 1, src.size);
    memory_index startSizeClamped = ClampDown(first, endSizeClamped);
    StringNewLen(dst, (src.buf + startSizeClamped), endSizeClamped - startSizeClamped);
}

// Note(liam): small visualizations. [...] = string, 0 = keep, x = remove,
//                                     | = size/count
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
    memory_index sizeClamped = ClampDown(size, src.size);
    memory_index startPos = src.size - sizeClamped;
    StringNewLen(dst, (src.buf + startPos), sizeClamped);
}

// Note(liam): [xx|00000]
void
StringSkipFront(StringData *dst, StringData src, memory_index count)
{
    memory_index countClamped = ClampDown(count, src.size);
    StringNewLen(dst, (src.buf + countClamped), src.size);
}

// Note(liam): [00000|xx]
void
StringSkipBack(StringData *dst, StringData src, memory_index count)
{
    memory_index countClamped = ClampDown(src.size, count);
    memory_index endPos = src.size - countClamped;
    StringNewLen(dst, src.buf, endPos);
}

void
StringPrint_(StringData s, FILE *stream)
{
    while (s.size--)
    {
        putc(*(s.buf++), stream);
    }
}

void
StringPrint(StringData s)
{
    StringPrint_(s, stdout);
}

void
StringPrintn(StringData s)
{
    StringPrint(s);
    putc('\n', stdout);
}

void
StringListPrint_(StringList l, FILE *stream, char end)
{
    if (l.first == l.last)
    {
        if (l.first)
        {
            StringPrint_(l.first->str, stream);
            putc(end, stream);
        }
    }
    else
    {
        for (StringNode *current = l.first;
                current != NULL;
                current = current->next)
        {
            StringPrint_(current->str, stream);
            putc(end, stream);
        }
    }
}

void
StringListPrint(StringList l)
{
    StringListPrint_(l, stdout, null);
}

void
StringListPrintn(StringList l)
{
    StringListPrint_(l, stdout, '\n');
    putc('\n', stdout);
}

void
StringListPrintln(StringList l)
{
    StringListPrint(l);
    putc('\n', stdout);
}

void
StringListMap(StringList l, void(*fn)(StringData))
{
    if (l.first == l.last)
    {
        if (l.first)
        {
            fn(l.first->str);
        }
    }
    else
    {
        for (StringNode *current = l.first;
                current != NULL;
                current = current->next)
        {
            fn(current->str);
        }
    }
}

// NOTE(liam): being extra apparent, this function assumes user will
// take care of allocating the node.
void
StringListPush_(StringList *list, StringData sd, StringNode *node_alloced)
{
    node_alloced->str = sd;

    if (list->first == list->last)
    {
        // NOTE(liam): first/second value.
        if (!list->first)
        {
            list->first = node_alloced;
            list->last = node_alloced;
        }
        else
        {
            list->first->next = node_alloced;
            list->last = node_alloced;
        }
    }
    else
    {
        StringNode *current = list->first;
        while (current != list->last)
        {
            current = current->next;
        }
        current->next = node_alloced;
        list->last = current->next;
    }
    list->nodeCount++;
    list->size += sd.size;
}

void
StringListPush(Arena *arena, StringList *list, StringData sd)
{
    StringNode* node = PushStruct(arena, struct StringNode);
    StringListPush_(list, sd, node);
}

StringData
StringListJoin(Arena *arena_astmp, StringList *list, StringJoin *join_optional)
{
    local StringJoin join_empty = {};
    StringJoin *join = join_optional;
    if (!join)
    {
        join = &join_empty;
    }

    memory_index size = (join->pre.size +
                        join->post.size +
                        join->mid.size * (list->nodeCount - 1) +
                        list->size);

    ArenaTemp tmpArena = ArenaScratchCreate(arena_astmp);

    string str = PushArray(arena_astmp, uint8, size + 1);
    string ptr = str;

    // NOTE(liam): pre
    MemoryCopy(ptr, join->pre.buf, join->pre.size);
    ptr += join->pre.size;

    // NOTE(liam): mid
    bool32 is_mid = 0;
    for (StringNode *node = list->first;
            node != 0;
            node = node->next)
    {
        if (is_mid)
        {
            MemoryCopy(ptr, join->mid.buf, join->mid.size);
            ptr += join->mid.size;
        }

        MemoryCopy(ptr, node->str.buf, node->str.size);
        ptr += node->str.size;

        is_mid = 1;
    }

    // NOTE(liam): post
    MemoryCopy(ptr, join->post.buf, join->post.size);
    ptr += join->post.size;

    *ptr = 0;
    StringData res = {0};
    StringNewLen(&res, str, size);
    ArenaScratchFree(tmpArena);
    return(res);
}

StringList
StringSplit(Arena *arena, StringData sd, char *split_every_chars)
{
    StringList res = {0};
    StringData buf = {0};
    memory_index lastSplitIndex = 0;
    uint32 count = StringLength((string)split_every_chars);

    for (memory_index p = 0; p < sd.size; p++)
    {
        uint8 byte = *(sd.buf + p);
        for (uint32 i = 0; i < count; i++)
        {
            if (byte == split_every_chars[i])
            {
                StringSlice(&buf, sd, lastSplitIndex, p - 1);
                StringListPush(arena, &res, buf);
                lastSplitIndex = p + 1;
            }
        }
    }

    if (lastSplitIndex < sd.size)
    {
        StringSkipFront(&buf, sd, lastSplitIndex);
        StringListPush(arena, &res, buf);
    }

    return(res);
}

StringData
StringPushfv(Arena *arena_astmp, char *fmt, va_list args)
{
    va_list args2;
    va_copy(args2, args);

    memory_index bufSize = 1024;
    ArenaTemp tmp = ArenaScratchCreate(arena_astmp);

    string buf = PushArray(arena_astmp, uint8, bufSize);
    memory_index actualSize = vsnprintf((char*)buf, bufSize, fmt, args);

    StringData res = {0};
    if (actualSize < bufSize)
    {
        //ArenaPop(arena, bufSize - actualSize - 1);
        StringNewLen(&res, buf, actualSize);
    }
    else
    {
        //ArenaPop(arena, bufSize);
        string fixedBuf = PushArray(arena_astmp, uint8, actualSize + 1);
        memory_index finalSize = vsnprintf((char*)fixedBuf, actualSize + 1, fmt, args2);
        StringNewLen(&res, fixedBuf, finalSize);
    }

    va_end(args2);

    ArenaScratchFree(tmp);
    return(res);
}

StringData
StringPushf(Arena *arena_astmp, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    StringData res = StringPushfv(arena_astmp, fmt, args);
    va_end(args);
    return(res);
}

void
StringListPushf(Arena *arena_astmp, StringList *list, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    StringData str = StringPushfv(arena_astmp, fmt, args);
    va_end(args);
    StringListPush(arena_astmp, list, str);
}

bool32
StringMatch(StringData a, StringData b, StringMatchFlags flags_optional)
{
    bool32 res = false;

    if (a.size == b.size)
    {
        res = true;
        bool32 case_sens = ((flags_optional & StringMatchFlag_CaseSensitive) != 0);
        for (memory_index i = 0;
                i < a.size;
                i++)
        {
            uint8 ac = a.buf[i];
            uint8 bc = b.buf[i];
            if (!case_sens)
            {
                ac = CharUpper(ac);
                bc = CharUpper(bc);
            }
            if (ac != bc)
            {
                res = false;
                break;
            }
        }

    }
    return(res);
}

// NOTE(liam): decoding FROM UTF32.
uint32
StringDecodeUTF8(uint32 *dst, uint8 *src)
{
    // NOTE(liam): remember that the decoder targets only the
    // dereferenced value of dst, and not any of its neighbors.
    // So I would need to increment that pointer when decoding
    // a longer string.
    *dst = 0;
    uint32 res = 0; // codepoint output

    if (src[0] <= 0x7f)
    {
        // NOTE(liam): 0-127 direct mapping
        *dst = src[0];
        res = 1;
    }
    else if ((src[0] & 0xE0) == 0xC0)
    {
        *dst = (((src[0] & 0x1F) << 6)  |
                 (src[1] & 0x3F));
        res = 2;
    }
    else if ((src[0] & 0xF0) == 0xE0)
    {
        *dst = (((src[0] & 0x0F) << 12) |
                ((src[1] & 0x3F) << 6)  |
                 (src[2] & 0x3F));
        res = 3;

    }
    else if ((src[0] & 0xF8) == 0xF0)
    {
        *dst = (((src[0] & 0x07) << 18) |
                ((src[1] & 0x3F) << 12) |
                ((src[2] & 0x3F) << 6)  |
                 (src[3] & 0x3F));
        res = 4;
    }
    return(res);
}

void
StringEncodeUTF8(uint8 *dst, uint32 codepoint)
{
    // NOTE(liam): directly encodes codepoint into dst.
    if (codepoint <= 0x7f)
    {
        dst[0] = codepoint;
    }
    else if (codepoint <= 0x7ff)
    {
        dst[0] = 0xC0 | (codepoint >> 6);
        dst[1] = 0x80 | (codepoint & 0x3F);
    }
    else if (codepoint <= 0xffff)
    {
        dst[0] = 0xC0 | (codepoint >> 12);
        dst[1] = 0x80 | ((codepoint >> 6) & 0x3F);
        dst[2] = 0x80 | (codepoint & 0x3F);
    }
    else if (codepoint <= 0x10ffff)
    {
        dst[0] = 0xC0 | (codepoint >> 18);
        dst[1] = 0x80 | ((codepoint >> 12) & 0x3F);
        dst[2] = 0x80 | ((codepoint >> 6) & 0x3F);
        dst[3] = 0x80 | (codepoint & 0x3F);
    }
}

StringDecode
StringDecodeUTF16(uint16 *str, memory_index cap)
{
    StringDecode res = { '#', 1 };
    uint16 x = str[0];
    if (x < 0xD800 || 0xDFFF < x)
    {
        res.codepoint = x;
    }
    else if (cap >= 2)
    {
        uint16 y = str[1];
        if (0xD800 <= x && x < 0xDC00 &&
            0xDC00 <= y && y < 0xE000)
        {
            uint16 xj = x - 0xD800;
            uint16 yj = y - 0xDC00;
            uint32 xy = (xj << 10) | yj;
            res.codepoint = xy + 0x10000;
            res.length = 2;
        }
    }
    return(res);
}

uint32
StringEncodeUTF16(uint16 *dst, uint32 codepoint)
{
    uint32 size = 0;
    if (codepoint < 0x10000)
    {
        dst[0] = codepoint;
        size = 1;
    }
    else
    {
        uint32 cpj = codepoint - 0x10000;
        dst[0] = (cpj >> 10) + 0xD800;
        dst[1] = (cpj & 0x3FF) + 0xDC00;
        size = 2;
    }
    return(size);
}

/*****************/

/*String32Data*/
/*StringConvert32(Arena *arena, StringData sd)*/
/*{*/
/*    uint32 *mem = PushArray(arena, uint32, sd.size + 1);*/
/**/
/*    uint32 *dptr = mem;*/
/*    memory_index pos = 0;*/
/*    while (pos < sd.size)*/
/*    {*/
/*        StringDecode decode = StringDecodeUTF8(dptr, *(sd.buf + pos));*/
/*        *dptr = decode.codepoint;*/
/*        pos += decode.length;*/
/*        dptr += 1;*/
/*    }*/
/*    *dptr = 0;*/
/**/
/*    memory_index allocCount = sd.size + 1;*/
/*    memory_index stringCount = (memory_index)(*(dptr - mem));*/
/*    memory_index unusedCount = allocCount - stringCount - 1;*/
/*    ArenaPop(arena, unusedCount * sizeof(*mem));*/
/**/
/*    String32Data res = {mem, stringCount};*/
/*    return(res);*/
/*}*/
/**/
/*// NOTE(liam): implicit target to utf8*/
/*StringData*/
/*String32Convert(Arena *arena, String32Data sd)*/
/*{*/
/*    uint8 *mem = PushArray(arena, uint8, sd.size * 4 + 1);*/
/**/
/*    uint8 *dptr = mem;*/
/*    memory_index pos = 0;*/
/*    while (pos < sd.size)*/
/*    {*/
/*        uint32 size = StringEncodeUTF8(dptr, *(sd.buf + pos++));*/
/*        dptr += size;*/
/*    }*/
/*    *dptr = 0;*/
/**/
/*    memory_index allocCount = sd.size * 4 + 1;*/
/*    memory_index stringCount = (memory_index)(dptr - mem);*/
/*    memory_index unusedCount = allocCount - stringCount - 1;*/
/*    ArenaPop(arena, unusedCount * sizeof(*mem));*/
/**/
/*    StringData res = {mem, stringCount};*/
/*    return(res);*/
/**/
/*}*/
/**/
/*String16Data*/
/*StringConvert16(Arena *arena, StringData sd)*/
/*{*/
/*    uint16 *mem = PushArray(arena, uint16, sd.size * 2 + 1);*/
/**/
/*    uint16 *dptr = mem;*/
/*    memory_index pos = 0;*/
/*    while (pos < sd.size)*/
/*    {*/
/*        StringDecode decode = StringDecodeUTF8(dptr, *(sd.buf + pos));*/
/*        uint32 encodedSize = StringEncodeUTF16(dptr, decode.codepoint);*/
/*        pos += decode.length;*/
/*        dptr += encodedSize;*/
/*    }*/
/*    *dptr = 0;*/
/**/
/*    memory_index allocCount = sd.size * 2 + 1;*/
/*    memory_index stringCount = (memory_index)(dptr - mem);*/
/*    memory_index unusedCount = allocCount - stringCount - 1;*/
/*    ArenaPop(arena, unusedCount * sizeof(*mem));*/
/**/
/*    String16Data res = {mem, stringCount};*/
/*    return(res);*/
/*}*/
/**/
/*StringData*/
/*String16Convert(Arena *arena, String16Data sd)*/
/*{*/
/*    uint8 *mem = PushArray(arena, uint16, sd.size * 3 + 1);*/
/**/
/*    uint8 *dptr = mem;*/
/*    memory_index pos = 0;*/
/*    while (pos < sd.size)*/
/*    {*/
/*        StringDecode decode = StringDecodeUTF16(dptr, *(sd.buf + pos));*/
/*        uint32 encodedSize = StringEncodeUTF8(dptr, decode.codepoint);*/
/*        pos += decode.length;*/
/*        dptr += encodedSize;*/
/*    }*/
/*    *dptr = 0;*/
/**/
/*    memory_index allocCount = sd.size * 3 + 1;*/
/*    memory_index stringCount = (memory_index)(dptr - mem);*/
/*    memory_index unusedCount = allocCount - stringCount - 1;*/
/*    ArenaPop(arena, unusedCount * sizeof(*mem));*/
/**/
/*    StringData res = {mem, stringCount};*/
/*    return(res);*/
/*}*/
