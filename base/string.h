#ifndef STRING_H
#define STRING_H

// NOTE(liam): use temp buffer, ZII if possible
#include "arena.h"

typedef struct StringNode {
    uint8* data;
    memory_index size;
    memory_index capacity;
    struct StringNode* next;
} StringNode;

typedef struct String {
    StringNode* first;
    memory_index count;
} String;

inline memory_index
StringConstSize(const uint8* inp)
{
    memory_index size = 0;

    while ((inp + size) != '\0') { size++; }

    return(size);
}

// push to a zero-initialized string.
void StringPush(Arena*, String, const uint8*);

void StringClear(String);

void StringPush(Arena* arena, String s, const uint8* inp)
{
    memory_index inputSize = StringConstSize(inp);

    /*
     * ["hell"] -> ["o world"]
     */
    if (!s->count)
    {
        StringNode first = s->first;
        first = PushStruct(arena, StringNode);
        first->size = inputSize;
        first->capacity = inputSize;
        first->data = PushArray(arena, uint8, first->capacity + 1);
    }
    StringNode current = s->first;
    while (current) {
        if (current->size + inputSize <= current->capacity)
        {
            // TODO(liam): insertion possible. append to current node.
            uint32 currentPos = 0;
            while (inputSize--)
            {
                *(current->data + current->size) = *(inp + currentPos++);
                current->size++;
            }
            break;
        }
        current = current->next;
    }
}

#endif // STRING_H
