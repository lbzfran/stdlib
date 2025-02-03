
#include "string.h"

memory_index
StringConstSize(const int8* inp)
{
    memory_index size = 0;

    while (*(inp + size) != '\0') { size++; }

    return(size);
}

void StringPush(Arena* arena, String* s, const int8* inp)
{
    memory_index inputSize = StringConstSize(inp);

    /*
     * ["hel"] -> ["o world"xxxxx] + [" hehe"] =
     * ["hel"] -> ["o world hehe"]
     *
     */
    if (!s->count)
    {
        StringNode* first = s->first;
        first = PushStruct(arena, StringNode);
        first->capacity = inputSize + 1;
        first->data = PushArray(arena, uint8, first->capacity);

        while (*(inp + first->size) != '\0') {
            first->size++;
        }
        *first->data = *inp;
    }
    else
    {
        StringNode* current = s->first;
        while (current->next) {
            if (current->size + inputSize <= current->capacity)
            {
                // TODO(liam): insertion possible. append to current node.
                uint32 currentPos = 0;
                while (inputSize--)
                {
                    *(current->data + current->size++) = *(inp + currentPos++);
                }
                break;
            }
            if (!current->next)
            {
                StringNode* next = current->next;
                next->size = inputSize;
                next->capacity = inputSize + 1;
                next->data = PushArray(arena, uint8, next->capacity);

                *next->data = *inp;


                break;
            }
            current = current->next;
        }
    }
}

void StringPrint(String* s)
{
    StringNode* current = s->first;
    while (current)
    {
        for (memory_index i = 0; i < current->size; i++)
        {
            printf("%d", *current->data);
        }
        current = current->next;
    }
    printf("\n");
}
