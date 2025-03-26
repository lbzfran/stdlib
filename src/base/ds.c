
#include "ds.h"

void uint32ArrayAppend(Arena *arena, uint32Array *a, uint32 x)
{
	if (a->size + 1 >= a->capacity)
	{
		uint32 newCap = Max(a->capacity * 2, 32);
		a->V = PushCopy(arena, newCap, a->V);
		a->capacity = newCap;
	}
	a->V[a->size++] = x;
}
