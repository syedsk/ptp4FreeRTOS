#ifndef FREERTOS_TIME_H
#define FREERTOS_TIME_H
#include <time.h>

typedef uint64_t u64;
typedef int64_t  s64;
typedef uint32_t u32;
typedef int32_t  s32;

/* Map the ktime to timespec conversion to ns_to_timespec function */
#define ktime_to_timespec(kt)           ns_to_timespec((kt).tv64)
        
/* Map the ktime to timespec conversion to ns_to_timespec function */
#define ktime_to_timespec64(kt)         ns_to_timespec64((kt).tv64)

/* Map the ktime to timeval conversion to ns_to_timeval function */
#define ktime_to_timeval(kt)            ns_to_timeval((kt).tv64)
        
/* Convert ktime to nanoseconds - NOP in the scalar storage format: */
#define ktime_to_ns(kt)                 ((kt).tv64)

#define NSEC_PER_SEC    1000000000L

/*      
 * Mode codes (timex.mode)
 */             
#define ADJ_OFFSET              0x0001  /* time offset */
#define ADJ_FREQUENCY           0x0002  /* frequency offset */
#define ADJ_MAXERROR            0x0004  /* maximum time error */
#define ADJ_ESTERROR            0x0008  /* estimated time error */
#define ADJ_STATUS              0x0010  /* clock status */
#define ADJ_TIMECONST           0x0020  /* pll time constant */
#define ADJ_TAI                 0x0080  /* set TAI offset */
#define ADJ_SETOFFSET           0x0100  /* add 'time' to current time */
#define ADJ_MICRO               0x1000  /* select microsecond resolution */
#define ADJ_NANO                0x2000  /* select nanosecond resolution */
#define ADJ_TICK                0x4000  /* tick value */

#define STA_INS			0x0010  /* insert leap (rw) */
#define STA_DEL			0x0020  /* delete leap (rw) */
#define STA_UNSYNC		0x0040  /* clock unsynchronized (rw) */
#define STA_FREQHOLD		0x0080  /* hold frequency (rw) */

union ktime {   
        s64     tv64; 
};
typedef union ktime ktime_t;

struct timex {
	int  modes;      /* Mode selector */
	long freq;       /* Frequency offset; see NOTES for units */
	struct timeval time;
	int  status;     /* Clock command/status */
	long constant;   /* PLL (phase-locked loop) time constant */
	long tolerance;
};

ktime_t ktime_set(const long secs, const unsigned long nsecs);
/* convert a timespec to ktime_t format: */
ktime_t timespec_to_ktime(struct timespec ts);

long div_s64_rem(s64 dividend, s64 divisor, s32 *remainder);

void set_normalized_timespec(struct timespec *ts, time_t sec, s64 nsec);

struct timespec ns_to_timespec(const s64 nsec);

int scaled_ppm_to_ppb(long ppm);
#endif
