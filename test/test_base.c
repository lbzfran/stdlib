
// unit testing for each section of the base.

#include "base/base.h"

struct TestUnit {
    uint32 pass;
    uint32 total;


};


int main(void)
{
    struct Test results = {0};

    printf("RESULT: %d out of %d failed.\n", results.fails, results.total);
    return 0;
}
