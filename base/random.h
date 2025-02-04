/*
 * ---------------
 * Liam Bagabag
 * Version: 2.0.0
 * requires: n/a
 * ---------------
 */
#ifndef RANDOM_H
#define RANDOM_H

#include "def.h"
#include "math.h"

static const uint32 MaxRandomNumber = 32768;
static const uint32 RandomSalt = 12345; // arbitrary value

typedef struct random_series {
    uint32 index;
} RandomSeries;

RandomSeries RandomSeed(uint32 value);
uint32 RandomNextInt(RandomSeries* series);
uint32 RandomChoice(RandomSeries* series, uint32 N); // [0, N)
float32 RandomUnilateral(RandomSeries* series); // [0,1]
float32 RandomBilateral(RandomSeries* series); // [-1,1]
float32 RandomBetween(RandomSeries* series, float32 min, float32 max); // (min, max)

inline RandomSeries
RandomSeed(uint32 value)
{
    RandomSeries series;
    series.index = value * 1103515245 + RandomSalt;

    return(series);
}

inline uint32
RandomNextInt(RandomSeries* series)
{
    // returns current value of index, and increments index position.
    uint32 res = (uint32)(series->index/65536);
    series->index *= 1103515245 + RandomSalt;

    return(res);
}

inline uint32
RandomChoice(RandomSeries* series, uint32 N)
{
    // random choice between [0, N).

    uint32 res = (RandomNextInt(series) % N);

    return(res);
}

inline float32
RandomUnilateral(RandomSeries* series)
{
    // range of [0 to 1].
    float32 div = 1.0f / (float32)MaxRandomNumber;
    float32 res = div * (float32)RandomNextInt(series);

    return(res);
}

inline float32
RandomBilateral(RandomSeries* series)
{
    // range of [-1 to 1].
    float32 res = 2.0f * RandomUnilateral(series) - 1.0f;

    return(res);
}

inline float32
RandomBetween(RandomSeries* series, float32 min, float32 max)
{
    /*float32 range = max - min;*/
    /*float32 res   = min + RandomUnilateral(series) * range;*/
    float32 res = Lerp(min, RandomUnilateral(series), max);

    return(res);
}

#endif //RANDOM_H
