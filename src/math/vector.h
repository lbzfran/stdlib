/*
 * ---------------
 * Liam Bagabag
 * Version: 2.0
 * require: none
 * ---------------
 */
#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H

#include "base/def.h"
#include "op.h"
#include "base/arena.h"

typedef struct Vector2u {
    uint32 x, y;
} Vector2u, Vec2u;

typedef struct Vector3u {
    uint32 x, y, z;
} Vector3u, Vec3u;

typedef struct Vector2f {
    float32 x, y;
} Vector2f, Vec2f;

typedef struct Vector3f {
    float32 x, y, z;
} Vector3f, Vec3f;


typedef struct ArrayVec3f {
    Vector3f *V;
    uint32 size;
    uint32 capacity;
} ArrayVec3f;

void ArrayVec3fPush(Arena *arena, ArrayVec3f *a, Vector3f x);

#define ArrayVec3fDump(A) do{\
    for (int i = 0; i < (A).size; i++){\
        printf("x: %f y: %f z: %f\n", (A).V[i].x, (A).V[i].y, (A).V[i].z);\
    };\
}while(0);

Vector2f Vector2Add(Vector2f a, Vector2f b);
Vector2f Vector2Sub(Vector2f a, Vector2f b);
Vector2f Vector2Mul(Vector2f a, float32 b);
float32 Vector2Magnitude(Vector2f a);
float32 Vector2Dot(Vector2f a, Vector2f b);
float32 Vector2Cross(Vector2f a, Vector2f b);

Vector3f Vector3Add(Vector3f a, Vector3f b);
Vector3f Vector3Sub(Vector3f a, Vector3f b);
Vector3f Vector3Mul(Vector3f a, float32 b);
float32 Vector3Magnitude(Vector3f a);
float32 Vector3Dot(Vector3f a, Vector3f b);
float32 Vector3Cross(Vector3f a, Vector3f b);

#endif
