
#include "op.h"
#include "vector.h"

Vector2
Vector2Add(Vector2 a, Vector2 b)
{
    Vector2 res = {
        .x = a.x + b.x,
        .y = a.y + b.y
    };

    return(res);
}

Vector2
Vector2Sub(Vector2 a, Vector2 b)
{
    Vector2 res = {
        .x = a.x - b.x,
        .y = a.y - b.y
    };

    return(res);
}

Vector2
Vector2Mul(Vector2 a, float32 b)
{
    Vector2 res = {
        .x = a.x * b,
        .y = a.y * b
    };

    return(res);
}

float32
Vector2Magnitude(Vector2 a)
{
    float32 res = SqrtFloat32(a.x * a.x + a.y * a.y);

    return(res);
}


float32
Vector2Dot(Vector2 a, Vector2 b)
{
    float32 res = (a.x * b.x) + (a.y * b.y);

    return(res);
}

float32
Vector2Cross(Vector2 a, Vector2 b)
{
    float32 res = (a.x * b.y) - (a.y * b.x);

    return(res);
}

Vector3
Vector3Add(Vector3 a, Vector3 b)
{
    Vector3 res = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };

    return(res);
}

Vector3
Vector3Sub(Vector3 a, Vector3 b)
{
    Vector3 res = {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };

    return(res);
}

Vector3
Vector3Mul(Vector3 a, float32 b)
{
    Vector3 res = {
        .x = a.x * b,
        .y = a.y * b,
        .z = a.z * b
    };

    return(res);
}

float32
Vector3Dot(Vector3 a, Vector3 b)
{
    float32 res = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);

    return(res);
}

float32
Vector3Cross(Vector3 a, Vector3 b)
{
    float32 res = (a.y * b.z - a.z * b.y) + (a.z * b.x - a.x * b.z) + (a.x * b.y - a.y * b.x);

    return(res);
}

float32
Vector3Magnitude(Vector3 a)
{
    float32 res = SqrtFloat32(a.x * a.x + a.y * a.y + a.z * a.z);

    return(res);
}
