#ifndef STRING_H
#define STRING_H

// NOTE(liam): use temp buffer.
#include "arena.h"

typedef struct StringData {
    // NOTE(liam): cap excludes header and null term.
    // since we're treating this as immutable:
    // cap == (size == used).
    memory_index size;
    uint8 *buf;
} StringData;

typedef struct String16Data {
    memory_index size;
    uint16 *buf;
} String16Data;

typedef struct String32Data {
    memory_index size;
    uint32 *buf;
} String32Data;

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

typedef struct StringJoin {
    StringData pre;
    StringData mid;
    StringData post;
} StringJoin;

typedef uint32 StringMatchFlags;
enum {
    StringMatchFlag_CaseSensitive = 1 << 0,
};

// NOTE(liam): helpers
void MemoryCopy(void *dst, void *src, memory_index size);
memory_index StringLength(uint8 *str);
uint8 CharUpper(uint8 c);
uint8 CharLower(uint8 c);

#define StringLiteral(sd) (sd.buf)

// NOTE(liam): basics
// Bit of a hack, but I'm casting the str input as null pointer to avoid
// the compiler complaining about the the passing char pointer to uint pointer.
uint8* StringNewLen(StringData *sd, void *str, memory_index size);
uint8* StringNew(StringData *sd, void *str);
uint8* StringNewRange(StringData *sd, uint8 *first, uint8 *last_optional);

// NOTE(liam): manips
void StringSlice(StringData *dst, StringData src, memory_index first, memory_index last);
void StringPrefix(StringData *dst, StringData src, memory_index size);
void StringPostfix(StringData *dst, StringData src, memory_index size);
void StringSkipFront(StringData *dst, StringData src, memory_index count);
void StringSkipBack(StringData *dst, StringData src, memory_index count);

// NOTE(liam): prints
void StringPrint_(StringData s, FILE *stream);
void StringPrint(StringData s);
void StringPrintn(StringData s);
void StringListPrint_(StringList l, FILE *stream, char end_optional);
void StringListPrint(StringList l);
void StringListPrintn(StringList l);
void StringListPrintln(StringList l);
void StringListMap(StringList l, void(*fn)(StringData));

// NOTE(liam): list manips
void StringListPush_(StringList *list, StringData sd, StringNode *node_alloced);
void StringListPush(Arena *arena, StringList *list, StringData sd);
StringData StringListJoin(Arena *arena_astmp, StringList *list, StringJoin *join_optional);
StringList StringSplit(Arena *arena, StringData sd, char *split_every_chars);

// NOTE(liam): fmts
StringData StringPushfv(Arena *arena_astmp, char *fmt, va_list args);
StringData StringPushf(Arena *arena_astmp, char *fmt, ...);
void StringListPushf(Arena *arena_astmp, StringList *list, char *fmt, ...);

bool32 StringMatch(StringData a, StringData b, StringMatchFlags flags_optional);
// NOTE(liam): unicode conversions


/*StringDecode StringDecodeUTF8(uint8* str, memory_index cap);*/
/*uint32 StringEncodeUTF8(uint8* dst, uint32 codepoint);*/
uint32 StringDecodeUTF8(uint32 *dst, uint8 *src); // 8 to 32
uint32 StringEncodeUTF8(uint8 *dst, uint32 codepoint); // 32 to 8

uint32 StringDecodeUTF16(uint32 *dst, uint16 *src); // 8 to 32 to 16
uint32 StringEncodeUTF16(uint16 *dst, uint32 codepoint); // 16 to 32 to 8

String32Data StringConvert32(Arena *arena, StringData sd);
StringData String32Convert(Arena *arena, String32Data sd);
String16Data StringConvert16(Arena *arena, StringData sd);
StringData String16Convert(Arena *arena, String16Data sd);

#endif // STRING_H
