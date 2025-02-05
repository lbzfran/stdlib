#ifndef STRING_H
#define STRING_H

// NOTE(liam): use temp buffer.
#include "arena.h"

// NOTE(liam): I realized the implementation with type defining char * makes it difficult
// to make these strings not dynamically allocated, despite needing to be immutable.
typedef uint8* String;

typedef struct StringData {
    // NOTE(liam): cap excludes header and null term.
    // since we're treating this as immutable:
    // cap == (size == used).
    memory_index size;
    /*uint8 buf[];*/
    String buf;
} StringData;

typedef struct StringNode {
    struct StringNode *next;
    String string;
} StringNode;

typedef struct StringList {
    StringNode *first;
    StringNode *last;
    memory_index nodeCount;
    memory_index size;
} StringList;

// NOTE(liam): helpers
void StringCopy(void *dst, void *src, memory_index n);
memory_index StringLength(String str);
uint8 CharUpper(uint8 c);
uint8 CharLower(uint8 c);

// NOTE(liam): basics
// Bit of a hack, but I'm casting the str input as null pointer to avoid
// the compiler complaining about the the passing char pointer to uint pointer.
String StringNewLen(StringData* sd, void *str, memory_index size);
String StringNew(StringData* sd, void *str);
/*StringData* StringGetData(String s);*/

// NOTE(liam): manips
// I think the slicing is inefficient as is since it requires the arena,
// where I'd like to have the slices refer to the original string instead.
void StringSlice(StringData *dst, StringData *src, memory_index first, memory_index last);
void StringPrefix(StringData *dst, StringData *src, memory_index size);
void StringPostfix(StringData *dst, StringData *src, memory_index size);
void StringSkipFront(StringData *dst, StringData src, memory_index count);
void StringSkipBack(StringData *dst, StringData src, memory_index count);

#endif // STRING_H
