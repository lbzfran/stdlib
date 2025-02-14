
#include <Windows.h>
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

DateTime
TimeSysToDate(SYSTEMTIME st)
{
    DateTime res = {0};

    res.year = st.wYear;
    res.month = (uint8)st.wMonth;
    res.day = st.wDay;
    res.hour = st.wHour;
    res.min = st.wMinute;
    res.sec = st.wSecond;
    res.ms = st.wMilliseconds;

    return(res);
}

SYSTEMTIME
TimeDateToSys(DateTime *t)
{
    SYSTEMTIME res = {};

    res.wYear = t->year;
    res.wMonth = t->month;
    res.wDay = t->day;
    res.wHour = t->hour;
    res.wMinute = t->min;
    res.wSecond = t->sec;
    res.wMilliseconds = t->ms;

    return(res);
}

DenseTime
TimeFileToDense(FILETIME *ft)
{
    SYSTEMTIME systime = {0};

    FileTimeToSystemTime(ft, &systime);
    DateTime dt = TimeSysToDate(systime);
    DenseTime res = TimeDateToDense(dt);

    return(res);
}

DateTime
TimeUniversal(void)
{
    SYSTEMTIME systemTime = {0};
    GetSystemTime(&systemTime);
    DateTime res = TimeSysToDate(systemTime);
    return(res);
}

DateTime
TimeUniversalToLocal(DateTime t)
{
    SYSTEMTIME univSystemTime = TimeDateToSys(&t);
    FILETIME univFileTime = {0};
    SystemTimeToFileTime(&univSystemTime, &univFileTime);

    FILETIME localFileTime = {0};
    FileTimeToLocalFileTime(&univFileTime, &localFileTime);

    SYSTEMTIME localSystemTime = {0};
    FileTimeToSystemTime(&localFileTime, &localSystemTime);

    DateTime res = TimeSysToDate(localSystemTime);
    return(res);
}

DateTime
TimeLocalToUniversal(DateTime t)
{
    SYSTEMTIME localSystemTime = TimeDateToSys(&t);
    FILETIME localFileTime = {0};
    SystemTimeToFileTime(&localSystemTime, &localFileTime);

    FILETIME univFileTime = {0};
    LocalFileTimeToFileTime(&localFileTime, &univFileTime);

    SYSTEMTIME univSystemTime = {0};
    FileTimeToSystemTime(&univFileTime, &univSystemTime);

    DateTime res = TimeSysToDate(univSystemTime);
    return(res);
}
