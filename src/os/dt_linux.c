
#include <time.h>
#include "dt.h"

DateTime
TimeDenseToDate(DenseTime d)
{
    DateTime res = {0};

    res.ms = d % 1000;
    d /= 1000;

    res.sec = d % 61;
    d /= 61;

    res.min = d % 60;
    d /= 60;

    res.hour = d % 24;
    d /= 24;

    res.day = d % 31;
    d /= 31;

    res.month = d % 12;
    d /= 12;

    int32 yearEncoded = (int32)d;
    res.year = (yearEncoded - 0x8000);
    return(res);
}

DenseTime
TimeDateToDense(DateTime t)
{
    uint32 yearEncoded = (uint32)((int32)t.year + 0x8000);
    DenseTime res = 0;

    res += yearEncoded;

    res *= 12;
    res += t.month;

    res *= 31;
    res += t.day;

    res *= 24;
    res += t.hour;

    res *= 60;
    res += t.min;

    res *= 61;
    res += t.sec;

    res *= 1000;
    res += t.ms;

    return(res);
}

// NOTE(liam): since tm doesn't store ms,
// this func returns it.
uint64
TimeDateToTM(DateTime t, struct tm *dst)
{
    dst->tm_year = t.year;
    dst->tm_mon = t.month;
    dst->tm_mday = t.day;
    dst->tm_hour = t.hour;
    dst->tm_min = t.min;
    dst->tm_sec = t.sec;

    return(t.ms);
}

DateTime
TimeTMToDate(struct tm t, uint64 ms)
{
    DateTime res = {0};

    res.year = t.tm_year;
    res.month = (uint8)t.tm_mon;
    res.day = t.tm_mday;
    res.hour = t.tm_hour;
    res.min = t.tm_min;
    res.sec = t.tm_sec;
    res.ms = ms;

    return(res);
}

DateTime
TimeUniversal(void)
{
    struct timespec sysTimeSpec;
    clock_gettime(CLOCK_MONOTONIC_RAW, &sysTimeSpec);

    struct tm sysTime;
    gmtime_r(&sysTimeSpec.tv_sec, &sysTime);

    DateTime res = {
        .year = sysTime.tm_year,
        .month = (uint8)sysTime.tm_mon,
        .day = sysTime.tm_mday,
        .hour = sysTime.tm_hour,
        .min = sysTime.tm_min,
        .sec = sysTime.tm_sec,
        .ms = sysTimeSpec.tv_nsec
    };
    return(res);
}

DateTime
TimeUniversalToLocal(DateTime t)
{
    struct tm sysTime;
    uint64 ms = TimeDateToTM(t, &sysTime);
    time_t univTime = mktime(&sysTime);

    struct tm localTime;
    localtime_r(&univTime, &localTime);

    DateTime res = TimeTMToDate(localTime, ms);
    return(res);
}

DateTime
TimeLocalToUniversal(DateTime t)
{
    struct tm sysTime;
    uint64 ms = TimeDateToTM(t, &sysTime);
    time_t localTime = mktime(&sysTime);

    struct tm univTime;
    gmtime_r(&localTime, &univTime);

    DateTime res = TimeTMToDate(univTime, ms);
    return(res);
}
