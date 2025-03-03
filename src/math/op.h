#ifndef MATH_OP_H
#define MATH_OP_H

#include "base/def.h"
#include "limit.h"

int32 PowInt32(int32 base, int32 exp);

float32 AbsFloat32(float32 x);
float32 AbsFloat64(float32 x);

float32 SqrtFloat32(float32 x);
float64 SqrtFloat64(float64 x);

float32 SinFloat32(float32 x);
float32 CosFloat32(float32 x);
float32 TanFloat32(float32 x);

float64 SinFloat64(float64 x);
float64 CosFloat64(float64 x);
float64 TanFloat64(float64 x);

float32 Lerp(float32 a, float32 t, float32 b);
float32 Unlerp(float32 a, float32 x, float32 b);

#endif