/**
 * @file missing.h
 * @note Copyright (C) 2011 Richard Cochran <richardcochran@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * When glibc offers the syscall, this will go away.
 */
#ifndef HAVE_MISSING_H
#define HAVE_MISSING_H

#ifndef __FREERTOS__
#include <time.h>
#include <sys/syscall.h>
#include <sys/timex.h>
#else
#include "freeRTOS_time.h"
#include "timer_1588.h"
#include <lwip/arch.h>
#endif

#include <unistd.h>

#ifndef ADJ_TAI
#define ADJ_TAI 0x0080
#endif

#ifndef ADJ_NANO
#define ADJ_NANO 0x2000
#endif

#ifndef ADJ_SETOFFSET
#define ADJ_SETOFFSET 0x0100
#endif

#ifndef CLOCK_INVALID
#define CLOCK_INVALID -1
#endif

#define CLOCKFD 3
#define FD_TO_CLOCKID(fd)	((~(clockid_t) (fd) << 3) | CLOCKFD)
#define CLOCKID_TO_FD(clk)	((unsigned int) ~((clk) >> 3))

#ifndef HAVE_ONESTEP_SYNC
enum _missing_hwtstamp_tx_types {
	HWTSTAMP_TX_ONESTEP_SYNC = 2,
};
#endif

#ifndef SIOCGHWTSTAMP
#define SIOCGHWTSTAMP 0x89b1
#endif

#ifndef SO_SELECT_ERR_QUEUE
#define SO_SELECT_ERR_QUEUE 45
#endif

#ifndef HAVE_CLOCK_ADJTIME
#ifndef __FREERTOS__
static inline int clock_adjtime(clockid_t id, struct timex *tx)
{
	return syscall(__NR_clock_adjtime, id, tx);
}
#else

static inline int clock_adjtime(clockid_t id, struct timex *tx)
{
        int err = -EOPNOTSUPP;
	static int dialed_frequency;

        if (tx->modes & ADJ_SETOFFSET) {
                struct timespec ts;
                ktime_t kt;
                long delta;

		printf("%s set offset\n", __func__);
                ts.tv_sec  = tx->time.tv_sec;
                ts.tv_nsec = tx->time.tv_usec;

                if (!(tx->modes & ADJ_NANO))
                        ts.tv_nsec *= 1000;

                if ((unsigned long) ts.tv_nsec >= NSEC_PER_SEC)
                        return -EINVAL;

                kt = timespec_to_ktime(ts);
                delta = ktime_to_ns(kt);
                err = ptp_adjtime(id, delta);
        } else if (tx->modes & ADJ_FREQUENCY) {
                int ppb = scaled_ppm_to_ppb(tx->freq);
                if (ppb > 999999999 || ppb < -999999999)
                        return -ERANGE;
                err = ptp_adjfreq(0, ppb);
                dialed_frequency = tx->freq;
        } else if (tx->modes == 0) {
                tx->freq = dialed_frequency;
                err = 0;
        }

        return err;
}
#endif
#endif


#ifdef __uClinux__

static inline int clock_nanosleep(clockid_t clock_id, int flags,
				  const struct timespec *request,
				  struct timespec *remain)
{
	return syscall(__NR_clock_nanosleep, clock_id, flags, request, remain);
}

static inline int timerfd_create(int clockid, int flags)
{
	return syscall(__NR_timerfd_create, clockid, flags);
}

static inline int timerfd_settime(int fd, int flags,
				  const struct itimerspec *new_value,
				  struct itimerspec *old_value)
{
	return syscall(__NR_timerfd_settime, fd, flags, new_value, old_value);
}

#elif __FREERTOS__

int timerfd_create(int clockid, int flags);
int timerfd_settime(int fd, int flags,
				  const struct itimerspec *new_value,
				  struct itimerspec *old_value);

int timer_close(int fd);
static inline int clock_nanosleep(clockid_t clock_id, int flags,
				  const struct timespec *request,
				  struct timespec *remain)
{
	return 0; /* TBD Saurabh*/
}


#else
#include <sys/timerfd.h>
#endif
#endif
