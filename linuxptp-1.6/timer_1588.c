#include <xil_io.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freeRTOS_time.h"
#include "timer_1588.h"
#include "print.h"


static struct timespec timespec_add(struct timespec lhs,
                                                struct timespec rhs)
{       
        struct timespec ts_delta; 
        set_normalized_timespec(&ts_delta, lhs.tv_sec + rhs.tv_sec,
                                lhs.tv_nsec + rhs.tv_nsec);
        return ts_delta;
}


struct timespec xlnx_rtc_offset_read(long baseaddr)
{
	struct timespec ts;

        ts.tv_sec = Xil_In32(baseaddr + XTIMER1588_RTC_OFFSET_SEC_L);
	ts.tv_nsec = Xil_In32(baseaddr + XTIMER1588_RTC_OFFSET_NS);

	return ts;
}

static void xlnx_rtc_offset_write(long baseaddr,
                                  const struct timespec *ts)
{
        //xil_printf("%s: sec: %ld nsec: %ld\n", __func__, ts->tv_sec, ts->tv_nsec);

        Xil_Out32((baseaddr + XTIMER1588_RTC_OFFSET_SEC_H), 0);
        Xil_Out32((baseaddr + XTIMER1588_RTC_OFFSET_SEC_L), (ts->tv_sec));
        Xil_Out32((baseaddr + XTIMER1588_RTC_OFFSET_NS), ts->tv_nsec);
}

int ptp_adjfreq(int id, s32 ppb)
{
        int neg_adj = 0;
        u64 freq;
        u32 diff, incval;
	long addr;

	if (id == 0)
		addr = TEMAC1_BASEADDR;
	else
		addr = TEMAC2_BASEADDR;

        /* This number should be replaced by a call to get the frequency
	* from the device-tree. Currently assumes 125MHz
	*/
        incval = 0x800000;
        /* for 156.25 MHZ Ref clk the value is  incval = 0x800000; */

        if (ppb < 0) {
                neg_adj = 1;
                ppb = -ppb;
        }

        freq = incval;
        freq *= ppb;
        diff = freq/1000000000ULL;

        //printf("%s: adj: %d ppb: %d\n", __func__, diff, ppb);

        incval = neg_adj ? (incval - diff) : (incval + diff);
        //printf("%s: incval %d\n", __func__, incval);
        Xil_Out32((addr + XTIMER1588_RTC_INCREMENT), incval);
        return 0;

}

int ptp_adjtime(int id, s64 delta)
{
	struct timespec now, then = ns_to_timespec(delta);
	long addr;

	//printf("%s: ", __func__);
	//pr_debug("delta %10" PRId64, delta);

	if (id == 0)
		addr = TEMAC1_BASEADDR;
	else
		addr = TEMAC2_BASEADDR;

	now = xlnx_rtc_offset_read(addr);
	now = timespec_add(now, then);
	xlnx_rtc_offset_write(addr, (const struct timespec *)&now);

	return 0;
}

