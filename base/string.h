#ifndef STRING_H
#define STRING_H

// NOTE(liam): use temp buffer, ZII if possible
#include "arena.h"

typedef struct StringData {
    // NOTE(liam): cap excludes header and null term.
    // size = used.
    memory_index capacity;
    memory_index size;
    char buf[];
} StringData;

typedef char* String;

memory_index StringLength(char* str);
bool32 StringHasSpace(StringData* sData);

String StringNew(Arena* arena, const char* str);
StringData* StringDataAlloc(Arena* arena, memory_index length);
inline memory_index StringLength(String str);

#endif // STRING_H
