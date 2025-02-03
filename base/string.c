
#include "string.h"

memory_index
StringConstSize(int8* inp)
{
    memory_index size = 0;

    while (*(inp + size) != '\0') { size++; }

    return(size);
}

void StringPush(Arena* arena, String* s, int8* inp)
{
    memory_index inputSize = StringConstSize(inp);

    /*
     * ["hel"] -> ["o world"xxxxx] + [" hehe"] =
     * ["hel"] -> ["o world hehe"]
     *
     */
    if (!s->count)
    {
        StringNode* first = PushStruct(arena, StringNode);
        first->capacity = inputSize + 1;
        first->data = PushArray(arena, uint8, first->capacity);

        while (*(inp + first->size) != '\0') {
            *(first->data + first->size) = *(inp + first->size);
            first->size++;
        }
        *(first->data + first->size) = '\0';

        s->first = first;
        s->count++;
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

void StringPrint(String s)
{
    StringNode* current = s.first;
    while (current)
    {
        memory_index pos = 0;
        while (*(current->data + pos) != '\0') {
            printf("%c", *(current->data + pos++));
        }
        current = current->next;
    }
    printf("\n");
}
