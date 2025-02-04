
#include "string.h"

memory_index
StringConstSize(char* inp)
{
    memory_index size = 0;

    while (*(inp + size) != '\0') { size++; }

    return(size);
}

void StringPush(Arena* arena, String* s, char* inp)
{
    memory_index inputSize = StringConstSize(inp);
    printf("node check: %llu\n", s->count);
    /*
     * ["hel"] -> ["o world"xxxxx] + [" hehe"] =
     * ["hel"] -> ["o world hehe"]
     *
     */
    if (s->count == 0)
    {
        StringNode* first = PushStruct(arena, StringNode);
        first->capacity = Min(inputSize + 1, Kilobytes(1));
        first->data = PushArray(arena, int8, first->capacity);

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
        do {
            if (!current->next)
            {
                if (current->size + inputSize <= current->capacity)
                {
                    // TODO(liam): insertion possible. append to current node.
                    printf("can fit!\n");

                    memory_index pos = 0;
                    while (*(inp + pos) != '\0') {
                        *(current->data + current->size++) = *(inp + pos++);
                    }
                    *(current->data + current->size) = '\0';
                };

                printf("last position\n");
                StringNode* last = PushStruct(arena, StringNode);
                last->capacity = inputSize + 1;
                last->data = PushArray(arena, int8, last->capacity);


                while (*(inp + last->size - 1) != '\0') {
                    *(last->data + last->size) = *(inp + last->size);
                    last->size++;
                }
                *(last->data + last->size) = '\0';

                current->next = last;
                s->count++;

                break;
            }
            current = current->next;
        }
        while (current);


    }
}

void StringPrint(String* s)
{
    StringNode* current = s->first;
    while (current)
    {
        printf("current size of node: %llu\n", current->size);
        memory_index pos = 0;
        while (pos != current->size) {
            printf("%c", *(current->data + pos++));
        }
        current = current->next;
    }
    printf("\n");
}
