#ifndef STRING_H
#define STRING_H

// NOTE(liam): use temp buffer.
#include "arena.h"

// NOTE(liam): I realized the implementation with type defining char * makes it difficult
// because of the manipulation's heavy reliance of pointers and memory.
typedef struct StringData {
    // NOTE(liam): cap excludes header and null term.
    // size = used.
    memory_index capacity;
    memory_index size;
    uint8 buf[];
} StringData;

typedef uint8* String;

// NOTE(liam): helpers
void StringCopy(void *dst, void *src, memory_index n);
memory_index StringLength(String str);
uint8 CharUpper(uint8 c);
uint8 CharLower(uint8 c);

// NOTE(liam): basics
String StringNewLen(Arena *arena, void *str, memory_index size);
String StringNew(Arena *arena, void *str);
StringData* StringDataAlloc(Arena *arena, memory_index length);
StringData* StringGetData(String s);

// NOTE(liam): manips
String StringSlice(Arena *arena, String s, memory_index first, memory_index last);
String StringPrefix(Arena *arena, String s, memory_index last);
String StringPostfix(Arena *arena, String s, memory_index first);


#endif // STRING_H
