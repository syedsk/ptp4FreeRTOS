#include "rtc_clock.h"
#include "freeRTOS_time.h"
#include <xil_io.h>

static inline int is_leap_year(unsigned int year)
{
        return (!(year % 4) && (year % 100)) || !(year % 400);
}

int rtc_month_days(unsigned int month, unsigned int year)
{
        return rtc_days_in_month[month] + (is_leap_year(year) && month == 1);
}

/*
 * rtc_time_to_tm64 - Converts time64_t to rtc_time.
 * Convert seconds since 01-01-1970 00:00:00 to Gregorian date.
 */
void rtc_time64_to_tm(unsigned long time, struct rtc_time *tm)
{
	unsigned int month, year;
	unsigned long secs;
	int days;
	int remainder;

	/* time must be positive */
	/* days = div_s64(time, 86400); */
	days = div_s64_rem(time, 86400, &remainder);
	secs = time - (unsigned int) days * 86400;

	/* day of the week, 1970-01-01 was a Thursday */
	tm->tm_wday = (days + 4) % 7;

	year = 1970 + days / 365;
	days -= (year - 1970) * 365
		+ LEAPS_THRU_END_OF(year - 1)
		- LEAPS_THRU_END_OF(1970 - 1);
	if (days < 0) {
		year -= 1;
		days += 365 + is_leap_year(year);
	}
	tm->tm_year = year - 1900;
	tm->tm_yday = days + 1;

	for (month = 0; month < 11; month++) {
		int newdays;

		newdays = days - rtc_month_days(month, year);
		if (newdays < 0)
			break;
		days = newdays;
	}
	tm->tm_mon = month;
	tm->tm_mday = days + 1;

	tm->tm_hour = secs / 3600;
	secs -= tm->tm_hour * 3600;
	tm->tm_min = secs / 60;
	tm->tm_sec = secs - tm->tm_min * 60;

	tm->tm_isdst = 0;
}

int freeRTOS_clock_gettime( int flag, struct timespec *now)
{
/* TBD Saurabh CLOCK_MONOTONIC REALTIME*/
	struct rtc_time tm;

        rtc_time64_to_tm(Xil_In32(RTC_BASEADDR + RTC_CUR_TM), &tm);
	now->tv_sec = tm.tm_sec;
	now->tv_nsec = 0;
	return 0;
}
