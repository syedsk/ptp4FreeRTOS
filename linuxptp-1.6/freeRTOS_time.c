#include "freeRTOS_time.h"

/**
 * ktime_set - Set a ktime_t variable from a seconds/nanoseconds value
 * @secs:       seconds to set
 * @nsecs:      nanoseconds to set
 *      
 * Return: The ktime_t representation of the value.
 */
ktime_t ktime_set(const long secs, const unsigned long nsecs)
{
        return (ktime_t) { .tv64 = secs * NSEC_PER_SEC + (long)nsecs };
}
/* convert a timespec to ktime_t format: */
ktime_t timespec_to_ktime(struct timespec ts)
{
        return ktime_set(ts.tv_sec, ts.tv_nsec);
}

long div_s64_rem(s64 dividend, s64 divisor, s32 *remainder)
{       
        *remainder = dividend % divisor;
        return dividend / divisor;
}

void set_normalized_timespec(struct timespec *ts, time_t sec, s64 nsec)
{                               
        while (nsec >= NSEC_PER_SEC) {
 /*
 The following asm() prevents the compiler from
 optimising this loop into a modulo operation. See
 also __iter_div_u64_rem() in include/linux/time.h
 */
                asm("" : "+rm"(nsec));
                nsec -= NSEC_PER_SEC;
                ++sec;
        }
        while (nsec < 0) {
                asm("" : "+rm"(nsec));
                nsec += NSEC_PER_SEC;
                --sec;
        }
        ts->tv_sec = sec;
        ts->tv_nsec = nsec;
}

struct timespec ns_to_timespec(const s64 nsec)
{       
        struct timespec ts;
        s32 rem;
        
        if (!nsec)
                return (struct timespec) {0, 0};

        ts.tv_sec = div_s64_rem(nsec, NSEC_PER_SEC, &rem);
        if (rem < 0) {
                ts.tv_sec--;
                rem += NSEC_PER_SEC;
        }
        ts.tv_nsec = rem;

        return ts;
}       

int scaled_ppm_to_ppb(long ppm)
{
        s64 ppb = 1 + ppm;
        ppb *= 125;
        ppb >>= 13;
        return (int) ppb;
}

