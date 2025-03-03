
#include "base/def.h"
#include "math.h"
#include <math.h>

float32
InfFloat32(void)
{
    union{ float32 f; uint32 u; } r;
    r.u = 0x7f800000;
    return(r.f);
}

float32
InfNegativeFloat32(void)
{
    union{ float32 f; uint32 u; } r;
    r.u = 0xff800000;
    return(r.f);
}

float64
InfFloat64(void)
{
    union{ float64 f; uint64 u; } r;
    r.u = 0x7ff0000000000000;
    return(r.f);
}

float64
InfNegativeFloat64(void)
{
    union{ float64 f; uint64 u; } r;
    r.u = 0xfff0000000000000;
    return(r.f);
}

int32
PowInt32(int32 base, int32 exp)
{
    int32 res = 1;
    for (;;)
    {
        if (exp & 1) { res *= base; }
        exp >>= 1;
        if (!exp) { break; }
        base *= base;
    }
    return(res);
}

float32
AbsFloat32(float32 x)
{
    union{ float32 f; uint32 u; } r;
    r.f = x;
    r.u &= 0x7fffffff;
    return(r.f);
}

float32
AbsFloat64(float32 x)
{
    union{ float32 f; uint32 u; } r;
    r.f = x;
    r.u &= 0x7fffffffffffffff;
    return(r.f);
}

// TODO(liam): calculate these.
float32
SqrtFloat32(float32 x)
{
    // TODO(liam): test that this works.
    int32 start = 0, end = x;
    int32 mid = 0;

    float32 res = 0;

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

float32
SinFloat32(float32 x)
{

    return sinf(x);
}

float32
CosFloat32(float32 x)
{
    return cosf(x);
}

float32
TanFloat32(float32 x)
{
    return tanf(x);
}

float64
SqrtFloat64(float64 x)
{
    // TODO(liam): test that this works.
    int64 start = 0, end = x;
    int64 mid = 0;

    float64 res = 0;

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

float64
SinFloat64(float64 x)
{
    return sinf(x);
}

float64
CosFloat64(float64 x)
{
    return cosf(x);
}

float64
TanFloat64(float64 x)
{
    return tanf(x);
}

float32
Lerp(float32 a, float32 t, float32 b)
{
    float32 res = a + (b - a) * t;

    return(res);
}

float32
Unlerp(float32 a, float32 x, float32 b)
{
    float32 res = 0.f;
    if (a != b)
    {
        res = (x - a) / (b - a);
    }

    return(res);
}
