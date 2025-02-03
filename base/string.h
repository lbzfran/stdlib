#ifndef STRING_H
#define STRING_H

#include "arena.h"

// NOTE(liam): String8 is IMMUTABLE.

typedef struct String8 {
    uint8* str;
    uint64 size;
} String8;

typedef struct String8Node {
    struct String8Node* next;
    String8 string;
} String8Node;

typedef struct String8List {
    String8Node* first;
    String8Node* last;
    uint64 count;
    uint64 size;
} String8List;

typedef struct StringJoin {
    String8List pre;
    String8List mid;
    String8List post;
} StringJoin;

function String8 str8(uint8* str, uint64 size);
function String8 str8Range(uint8* first, uint8* opt);
function String8 str8CString(uint8* cstr);

#define str8Literal(s) str8((uint8*)(s), sizeof(s) - 1)

function String8 str8Prefix(String8 str, uint64 size);
function String8 str8Chop(String8 str, uint64 amount);
function String8 str8Postfix(String8 str, uint64 size);
function String8 str8Skip(String8 str, uint64 amount);
function String8 str8Substr(String8 str, uint64 first, uint64 opt);

#define str8Expand(s) (int)((s).size), ((s).str)

#endif
