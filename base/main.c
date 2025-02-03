
#include "base.h"

int main(void)
{
    Arena local_arena = {0};

    /*String8 test_string = str8((uint8)"howdy", 6);*/

    ArenaFree(&local_arena);
    return 0;
}
