#ifndef TIME_H
#define TIME_H

#include "base/def.h"

typedef uint64 DenseTime;

typedef struct DateTime {
    uint16 ms;
    uint8 sec;
    uint8 min;
    uint8 hour;
    uint8 day;
    uint8 month;
    int32 year;
} DateTime;

DateTime DenseToDateTime(DenseTime d);
DenseTime DateTimeToDense(DateTime t);

DateTime TimeNowUniversal(void);
DateTime TimeUniversalToLocal(DateTime *t);
DateTime TimeLocalToUniversal(DateTime *t);

#endif
