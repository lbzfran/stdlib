
#include "string.h"
#include <string.h>

// TODO(liam): i'm not sure if this is working the way I'm expecting it to.

inline memory_index
StringLength(String str)
{
    memory_index size = 0;

    while (*(str + size) != '\0') { size++; }

    return(size);
}

StringData*
StringDataAlloc(Arena* arena, memory_index length)
{
    StringData* s_data = (StringData*)PushSize(arena, sizeof(struct StringData) + length + 1);
    s_data->capacity = length + 1;
    s_data->size = length;
    return s_data;
}

String
StringNew(Arena* arena, const char* str)
{
    StringData* sData = NULL;

    if (str)
    {
        memory_index strLen = StringLength(str);
        sData = StringDataAlloc(arena, strLen);
        memcpy(&sData->buf, str, strLen);
        *(sData->buf + strLen) = '\0';
    }
    else
    {
        sData = StringDataAlloc(arena, 0);
        *sData->buf = '\0';
    }
    return sData->buf;
}
