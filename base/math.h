#ifndef TYPES_H
#define TYPES_H

# include <stdio.h>
# include <math.h>
// NOTE(liam): constants

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

// NOTE(liam): infinity
inline float32
InfFloat32(void)
{
    union{ float32 f; uint32 u; } r;
    r.u = 0x7f800000;
    return(r.f);
}

inline float32
InfNegativeFloat32(void)
{
    union{ float32 f; uint32 u; } r;
    r.u = 0xff800000;
    return(r.f);
}

inline float64
InfFloat64(void)
{
    union{ float64 f; uint64 u; } r;
    r.u = 0x7ff0000000000000;
    return(r.f);
}

inline float64
InfNegativeFloat64(void)
{
    union{ float64 f; uint64 u; } r;
    r.u = 0xfff0000000000000;
    return(r.f);
}

inline int32
PowInt32(int32 base, int32 exp)
{
    int32 res = 1;
    for (;;)
    {
        if (exp & 1) { result *= base; }
        exp >>= 1;
        if (!exp) { break; }
        base *= base;
    }
    return(res);
}

inline float32
AbsFloat32(float32 x)
{
    union{ float32 f; uint32 u; } r;
    r.f = x;
    r.u &= 0x7fffffff;
    return(r.f);
}

inline float32
AbsFloat64(float32 x)
{
    union{ float32 f; uint32 u; } r;
    r.f = x;
    r.u &= 0x7fffffffffffffff;
    return(r.f);
}

// TODO(liam): calculate these.
inline float32
SqrtFloat32(float32 x)
{
    // TODO(liam): test that this works.
    int32 start = 0, end = x;
    int32 mid;

    float32 res;

    while (start <= end)
    {
        mid = (start + end) / 2;

        if (mid * mid == x)
        {
            res = mid;
            break;
        }

        if (mid * mid < x)
        {
            res = start;

            start = mid + 1;
        }
        else { end = mid - 1; }
    }

    float32 inc = 0.1f;
    for (int8 i = 0; i < 5; i++)
    {
        while (res * res <= x)
        {
            res += inc;
        }

        res = res - inc;
        inc = inc / 10;
    }

    return(res);
}

//TODO(liam): REPLACE THESE EVENTUALLY.

inline float32
SinFloat32(float32 x)
{
    float32 res = 0.0f;

    return sinf(x);
}

inline float32
CosFloat32(float32 x)
{
    return cosf(x);
}

inline float32
TanFloat32(float32 x)
{
    return tanf(x);
}

inline float64
SqrtFloat64(float64 x)
{
    // TODO(liam): test that this works.
    int64 start = 0, end = x;
    int64 mid;

    float64 res;

    while (start <= end)
    {
        mid = (start + end) / 2;

        if (mid * mid == x)
        {
            res = mid;
            break;
        }

        if (mid * mid < x)
        {
            res = start;

            start = mid + 1;
        }
        else { end = mid - 1; }
    }

    float64 inc = 0.1f;
    for (int8 i = 0; i < 5; i++)
    {
        while (res * res <= x)
        {
            res += inc;
        }

        res = res - inc;
        inc = inc / 10;
    }

    return(res);
}

inline float64
SinFloat64(float64 x)
{
    return sinf(x);
}

inline float64
CosFloat64(float64 x)
{
    return cosf(x);
}

inline float64
TanFloat64(float64 x)
{
    return tanf(x);
}

inline float32
Lerp(float32 a, float32 t, float32 b)
{
    float32 res = a + (b - a) * t;

    return(res);
}
inline float32
Unlerp(float32 a, float32 x, float32 b)
{
    float32 res = 0.f;
    if (a != b)
    {
        res = (x - a) / (b - a);
    }

    return(res);
}

#endif
