
#include "base/def.h"

// NOTE(liam): example for dll loading.
uint32
sum(uint32 *vals, uint32 count)
{
    uint32 res = 0;
    for (uint32 i = 0; i < count; i++)
    {
        res += vals[i];
    }
    return(res);
}
