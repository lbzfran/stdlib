
#include "string.h"

//TODO(liam): rethink if i should need this or not.
function String8 str8(uint8* str, uint64 size)
{
    String8 res = {
        .str = str,
        .size = size
    };

    return(res);
}

function void
str8ListPushExplicit(String8List* list, String8 string, String8Node* node_allocated)
{
    // NOTE(liam): manual allocation of memory.
    node_allocated->string = string;
    String8Node* current = list->first;
    // TODO(liam): convert SLLQueuePush(list->first,list->last,nodeMemory) into
    // its literal function.
    list->count++;
    list->size += string.size;
}
function void
str8ListPush(Arena* arena, String8List* list, String8 string)
{
    String8Node* node = PushStruct(arena, String8Node);
    str8ListPushExplicit(list, string, node);
}

function String8
str8Join(Arena* arena, String8List* list, StringJoin* optional_join)
{
    // NOTE(liam): set optional if null
    local StringJoin dummyJoin = {};
    StringJoin* join = optional_join;
    if (!join)
    {
        join = &dummyJoin;
    }

    uint64 size = (join->pre.size +
                    join->post.size +
                    join->mid.size * (list->count - 1) +
                    list->size);
}

function String8
str8Split(Arena* arena, String8 string, uint8* split_characters, uint32 count)
{

}

#include <stdarg.h>
function String8
str8Pushfv(Arena* arena, char* fmt, va_list args)
{
    va_list args2;
    va_copy(args2, args);

    uint64 bufSize = Kilobytes(1);
    uint8* buf = PushArray(arena, uint8, bufSize);
    uint64 actualSize = vsnprintf((char*)buf, bufSize, fmt, args);

    String8 res = {};
    if (actualSize < bufSize)
    {
        ArenaPop(arena, bufSize - actualSize - 1);
        res = str8(buf, actualSize);
    }
    else
    {
        ArenaPop(arena, bufSize);
        uint8* fixedBuf = PushArray(arena, uint8, actualSize + 1);
        uint64 finalSize = vsnprintf((char*)fixedBuf, actualSize + 1, fmt, args2);
        res = str8(fixedBuf, finalSize);
    }

    va_end(args2);

    return(res);
}

function String8
str8Pushf(Arena* arena, char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    String8 res = str8Pushf(arena, fmt, args);
    va_end(args);

    return(res);
}

function String8
str8ListPushf(Arena* arena, String8List* list, char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    String8 string = str8Pushfv(arena, fmt, args);
    va_end(args);

    str8ListPush(arena, list, string);
}

