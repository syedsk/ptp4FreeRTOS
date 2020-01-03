#ifndef _RTC_CLOCK_H_
#define _RTC_CLOCK_H_

#include <time.h>

#define CLOCK_MONOTONIC                 0x0
#define RTC_BASEADDR                    0xffa60000
#define RTC_CUR_TM			0x10
#define LEAPS_THRU_END_OF(y) ((y)/4 - (y)/100 + (y)/400)

static const unsigned char rtc_days_in_month[] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

struct rtc_time {
        int tm_sec;
        int tm_min;
        int tm_hour;
        int tm_mday;
        int tm_mon;
        int tm_year;
        int tm_wday;
        int tm_yday;
        int tm_isdst;
};

int freeRTOS_clock_gettime( int flag, struct timespec *now);

#endif
