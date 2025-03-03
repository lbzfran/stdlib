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

typedef struct Vector2 {
    float32 x, y;
} Vector2;

typedef struct Vector3 {
    float32 x, y, z;
} Vector3;

Vector2 Vector2Add(Vector2 a, Vector2 b);
Vector2 Vector2Sub(Vector2 a, Vector2 b);
Vector2 Vector2Mul(Vector2 a, float32 b);
float32 Vector2Magnitude(Vector2 a);
float32 Vector2Dot(Vector2 a, Vector2 b);
float32 Vector2Cross(Vector2 a, Vector2 b);

Vector3 Vector3Add(Vector3 a, Vector3 b);
Vector3 Vector3Sub(Vector3 a, Vector3 b);
Vector3 Vector3Mul(Vector3 a, float32 b);
float32 Vector3Magnitude(Vector3 a);
float32 Vector3Dot(Vector3 a, Vector3 b);
float32 Vector3Cross(Vector3 a, Vector3 b);

#endif
