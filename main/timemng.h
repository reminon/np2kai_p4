#pragma once
#include "compiler.h"

typedef struct {
    UINT16  year;
    UINT16  month;
    UINT16  week;
    UINT16  day;
    UINT16  hour;
    UINT16  minute;
    UINT16  second;
    UINT16  milli;
} _SYSTIME;

BRESULT timemng_gettime(_SYSTIME *systime);
