
#include "op.h"
#include "vector.h"

Vector2f
Vector2Add(Vector2f a, Vector2f b)
{
    Vector2f res = {
        .x = a.x + b.x,
        .y = a.y + b.y
    };

    return(res);
}

Vector2f
Vector2Sub(Vector2f a, Vector2f b)
{
    Vector2f res = {
        .x = a.x - b.x,
        .y = a.y - b.y
    };

    return(res);
}

Vector2f
Vector2Mul(Vector2f a, float32 b)
{
    Vector2f res = {
        .x = a.x * b,
        .y = a.y * b
    };

    return(res);
}

float32
Vector2Magnitude(Vector2f a)
{
    float32 res = SqrtFloat32(a.x * a.x + a.y * a.y);

    return(res);
}


float32
Vector2Dot(Vector2f a, Vector2f b)
{
    float32 res = (a.x * b.x) + (a.y * b.y);

    return(res);
}

float32
Vector2Cross(Vector2f a, Vector2f b)
{
    float32 res = (a.x * b.y) - (a.y * b.x);

    return(res);
}

Vector3f
Vector3Add(Vector3f a, Vector3f b)
{
    Vector3f res = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };

    return(res);
}

Vector3f
Vector3Sub(Vector3f a, Vector3f b)
{
    Vector3f res = {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };

    return(res);
}

Vector3f
Vector3Mul(Vector3f a, float32 b)
{
    Vector3f res = {
        .x = a.x * b,
        .y = a.y * b,
        .z = a.z * b
    };

    return(res);
}

float32
Vector3Dot(Vector3f a, Vector3f b)
{
    float32 res = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);

    return(res);
}

float32
Vector3Cross(Vector3f a, Vector3f b)
{
    float32 res = (a.y * b.z - a.z * b.y) + (a.z * b.x - a.x * b.z) + (a.x * b.y - a.y * b.x);

    return(res);
}

float32
Vector3Magnitude(Vector3f a)
{
    float32 res = SqrtFloat32(a.x * a.x + a.y * a.y + a.z * a.z);

    return(res);
}

void ArrayVec3fPush(Arena *arena, ArrayVec3f *a, Vector3f x)
{
	if (a->size + 1 >= a->capacity)
	{
		uint32 newCap = Max(a->capacity * 2, 32);
		/*a->V = (uint32 *)PushCopy(arena, newCap, a->V);*/
        Vector3f *newV = PushArray(arena, Vector3f, newCap);
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
