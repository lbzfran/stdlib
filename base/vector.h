/*
 * ---------------
 * Liam Bagabag
 * Version: 2.0
 * require: none (inline)
 * ---------------
 */
#ifndef VECTOR_H
#define VECTOR_H

#include "def.h"
#include "math.h"

typedef struct Vector2 {
    float32 x, y;
} Vector2;

typedef struct Vector3 {
    float32 x, y, z;
} Vector3;

Vector2 Vector2Add(Vector2 a, Vector2 b);
Vector2 Vector2Sub(Vector2 a, Vector2 b);
Vector2 Vector2Mul(Vector2 a, float32 b);
Vector2 Vector2Magnitude(Vector2 a, Vector2 b);
Vector2 Vector2Dot(Vector2 a, Vector2 b);
Vector2 Vector2Cross(Vector2 a, Vector2 b);

Vector3 Vector3Add(Vector3 a, Vector3 b);
Vector3 Vector3Sub(Vector3 a, Vector3 b);
Vector3 Vector3Mul(Vector3 a, float32 b);
Vector3 Vector3Magnitude(Vector3 a, Vector3 b);
Vector3 Vector3Dot(Vector3 a, Vector3 b);
Vector3 Vector3Cross(Vector3 a, Vector3 b);

inline Vector2
Vector2Add(Vector2 a, Vector2 b)
{
    Vector2 res = {
        .x = a.x + b.x,
        .y = a.y + b.y
    };

    return(res);
}

inline Vector2
Vector2Sub(Vector2 a, Vector2 b)
{
    Vector2 res = {
        .x = a.x - b.x,
        .y = a.y - b.y
    };

    return(res);
}

inline Vector2
Vector2Mul(Vector2 a, float32 b)
{
    Vector2 res = {
        .x = a.x * b,
        .y = a.y * b
    };

    return(res);
}

inline float
Vector2Magnitude(Vector2 a)
{
    float32 res = SqrtFloat32(a.x * a.x + a.y * a.y);

    return(res);
}


inline float
Vector2Dot(Vector2 a, Vector2 b)
{
    float32 res = (a.x * b.x) + (a.y * b.y);

    return(res);
}

inline float
Vector2Cross(Vector2 a, Vector2 b)
{
    float32 res = (a.x * b.y) - (a.y * b.x);

    return(res);
}

inline Vector3
Vector3Add(Vector3 a, Vector3 b)
{
    Vector3 res = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };

    return(res);
}

inline Vector3
Vector3Sub(Vector3 a, Vector3 b)
{
    Vector3 res = {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };

    return(res);
}

inline Vector3
Vector3Mul(Vector3 a, float32 b)
{
    Vector3 res = {
        .x = a.x * b,
        .y = a.y * b,
        .z = a.z * b
    };

    return(res);
}

inline float
Vector3Dot(Vector3 a, Vector3 b)
{
    float32 res = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);

    return(res);
}

inline float
Vector3Cross(Vector3 a, Vector3 b)
{
    float32 res = (a.y * b.z - a.z * b.y) + (a.z * b.x - a.x * b.z) + (a.x * b.y - a.y * b.x);

    return(res);
}

inline float
Vector3Magnitude(Vector3 a)
{
    float32 res = SqrtFloat32(a.x * a.x + a.y * a.y + a.z * a.z);

    return(res);
}

#endif
