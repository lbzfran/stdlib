
#include "ds.h"

void uint32ArrayAppend(Arena *arena, uint32Array *a, uint32 x)
{
	if (a->size + 1 >= a->capacity)
	{
		uint32 newCap = Max(a->capacity * 2, 32);
		/*a->V = (uint32 *)PushCopy(arena, newCap, a->V);*/
        uint32 *newV = PushArray(arena, uint32, newCap);
        if (a->V)
        {
            for (uint32 i = 0; i < a->size; i++)
            {
                newV[i] = a->V[i];
            }
        }

        a->V = newV;
		a->capacity = newCap;
	}
	a->V[a->size++] = x;
}
