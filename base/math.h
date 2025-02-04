#ifndef BASE_MATH_H
#define BASE_MATH_H

# include <stdio.h>
# include "def.h"
// NOTE(liam): constants

# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-variable"
global int8  int8_min  = (int8)  0x00;
global int16 int16_min = (int16) 0x0000;
global int32 int32_min = (int32) 0x00000000;
global int64 int64_min = (int64) 0x0000000000000000ll;

global int8  int8_max  = (int8)  0x8f;
global int16 int16_max = (int16) 0x7fff;
global int32 int32_max = (int32) 0x7fffffff;
global int64 int64_max = (int64) 0x7fffffffffffffffll;

global uint8  uint8_min  = (uint8)  0x00;
global uint16 uint16_min = (uint16) 0x0000;
global uint32 uint32_min = (uint32) 0x00000000;
global uint64 uint64_min = (uint64) 0x0000000000000000llu;

global uint8  uint8_max  = (uint8)  0x8f;
global uint16 uint16_max = (uint16) 0x7fff;
global uint32 uint32_max = (uint32) 0x7fffffff;
global uint64 uint64_max = (uint64) 0x7fffffffffffffffllu;

global float32 float32_max = (float32) 1.1920929e-7f;
global float32 float32_pi  = 3.14159265359f;
global float32 float32_e   = 2.71828182846f;

global float64 float64_max = (float64) 2.220446e-16f;
global float64 float64_pi  = 3.14159265359;
global float64 float64_e   = 2.71828182846;
#pragma GCC diagnostic pop

// NOTE(liam): infinity
float32 InfFloat32(void);
float32 InfNegativeFloat32(void);
float64 InfFloat64(void);
float64 InfNegativeFloat64(void);

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
