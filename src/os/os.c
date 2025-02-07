
#include "base/base.h"
#include "os.h"

typedef uint32 (*sum_t)(uint32*, uint32);

int main(void)
{
    StringData path = {0};
    StringNew(&path, "libtest.so");
    void* lib = LibOpen(path);

    sum_t sum = (sum_t)LibLoad(lib, "sum");

    uint32 v[] = { 8, 8, 8, 8 };
    printf("8 * 4 = %d\n", sum(v, 4));

    LibClose(lib);
    return 0;
}
