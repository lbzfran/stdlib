#ifndef STRING_H
#define STRING_H

// NOTE(liam): use temp buffer, ZII if possible
#include "arena.h"

typedef struct StringNode {
    int8* data;
    memory_index size;
    memory_index capacity;
    struct StringNode* next;
} StringNode;

typedef struct String {
    StringNode* first;
    memory_index count;
} String;

memory_index StringConstSize(char* inp);

// push to a zero-initialized string.
void StringPush(Arena*, String*, char*);
void StringPrint(String* s);

void StringClear(String*);

#endif // STRING_H
