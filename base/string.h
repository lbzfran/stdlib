#ifndef STRING_H
#define STRING_H

// NOTE(liam): use temp buffer.
#include "arena.h"

// NOTE(liam): directly name the datatype.
typedef uint8* string;

typedef struct StringData {
    // NOTE(liam): cap excludes header and null term.
    // since we're treating this as immutable:
    // cap == (size == used).
    memory_index size;
    string buf;
} StringData;

typedef struct StringNode {
    struct StringNode *next;
    StringData str;
} StringNode;

typedef struct StringList {
    StringNode *first;
    StringNode *last;
    memory_index nodeCount;
    memory_index size;
} StringList;

// NOTE(liam): helpers
void StringCopy(string dst, string src, memory_index size);
memory_index StringLength(string str);
uint8 CharUpper(uint8 c);
uint8 CharLower(uint8 c);

// NOTE(liam): basics
// Bit of a hack, but I'm casting the str input as null pointer to avoid
// the compiler complaining about the the passing char pointer to uint pointer.
string StringNewLen(StringData *sd, void *str, memory_index size);
string StringNew(StringData *sd, void *str);

// NOTE(liam): manips
void StringSlice(StringData *dst, StringData src, memory_index first, memory_index last);
void StringPrefix(StringData *dst, StringData src, memory_index size);
void StringPostfix(StringData *dst, StringData src, memory_index size);
void StringSkipFront(StringData *dst, StringData src, memory_index count);
void StringSkipBack(StringData *dst, StringData src, memory_index count);

void StringPrint(StringData s);

// NOTE(liam): list manips
void StringListPush(StringList *list, string s, StringNode *node_alloced);
void StringListPushArena(Arena *arena, StringList *list, string s);
StringData StringJoin(Arena *arena);
StringList StringSplit(Arena *arena);

// NOTE(liam): fmt
string StringPushfv(Arena *arena, char *fmt, va_list args);
string StringPushf(Arena *arena, char *fmt, ...);
string StringListPushf(Arena *arena, StringList *list, char *fmt, ...);

#endif // STRING_H
