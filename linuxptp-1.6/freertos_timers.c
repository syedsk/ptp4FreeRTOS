#include "stdlib.h"
#include <lwip/sockets.h>
#include <sys/queue.h>
#include <time.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "freertos_timers.h"

#define SEC_TO_MSEC(s) ((s)*1000L)
#define NSEC_TO_MSEC(ns) ((ns)/1000000L)

struct timer
{
	int fd;
	TimerHandle_t id;
	LIST_ENTRY(timer) list;
};

LIST_HEAD(timers_head, timer) timers;

int freertos_timers_init()
{
	LIST_INIT(&timers);
	return 0;
}

void timer_callback(TimerHandle_t xTimer)
{
	int fd = (int) pvTimerGetTimerID(xTimer);

	//printf("%s <----------\n", __func__);

	if ( lwip_post_timer_event(fd) < 0 )
	{
		printf("%s: cant post event to lwip\n", __func__);
	}
}

int timerfd_create(int clockid, int flags)
{
	struct timer *t, *titer, *lastt=NULL;
	char name[10];
        SYS_ARCH_DECL_PROTECT(lev);

	t = (struct timer *)mem_malloc(sizeof(struct timer));
	if(!t)
	{
		printf("cant create timer: malloc failed\n");
		return -1;
	}

	/* create dummy socket for timer */
	t->fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if(t->fd < 0 )
	{
		printf("cant alloc timer \n");
		return -1;
	}

	/* protect timers list */
        SYS_ARCH_PROTECT(lev);
	LIST_FOREACH(titer, &timers, list)
		lastt = titer;
	if(lastt)
		LIST_INSERT_AFTER(lastt, t, list);
	else
		LIST_INSERT_HEAD(&timers, t, list);

        SYS_ARCH_UNPROTECT(lev);

	sprintf(name, "timer%d", t->fd);
	t->id = xTimerCreate(name, pdMS_TO_TICKS(500), pdTRUE, (void *)t->fd, timer_callback );

	if(t->id == NULL)
	{
		printf("%s: Failed to create timer\n");
		close(t->fd);
		SYS_ARCH_PROTECT(lev);
		LIST_REMOVE(t, list);
		SYS_ARCH_UNPROTECT(lev);
		mem_free(t);
		return -1;
	}

	return t->fd;
}


static struct timer *get_timer_from_fd(int fd)
{
	struct timer *t = NULL;
        SYS_ARCH_DECL_PROTECT(lev);

	/* protect timers list */
        SYS_ARCH_PROTECT(lev);

	LIST_FOREACH(t, &timers, list) {
		if( t->fd == fd )
			break;
	}

        SYS_ARCH_UNPROTECT(lev);

	return t;
}

int is_timer_fd(int fd)
{
	struct timer *t = NULL;
        SYS_ARCH_DECL_PROTECT(lev);

	/* protect timers list */
        SYS_ARCH_PROTECT(lev);

	LIST_FOREACH(t, &timers, list) {
		if( t->fd == fd ) {
			SYS_ARCH_UNPROTECT(lev);
			return TRUE;
		}
	}

        SYS_ARCH_UNPROTECT(lev);
	return FALSE;
}

int timerfd_settime(int fd, int flags,
				  const struct itimerspec *new_value,
				  struct itimerspec *old_value)
{
	TimerHandle_t xtimer;
	int id;
	portTickType period;
	struct timer *t;

	t = get_timer_from_fd(fd);
	
	xtimer = t->id;

	if(xtimer == NULL)
	{
		printf("%s: INvalid timer\n", __func__);
	}

	#ifdef DEBUG
	printf("sec: %ld nsec: %ld\n", (new_value->it_value.tv_sec), (new_value->it_value.tv_nsec));
	printf("val: %ld\n", ( SEC_TO_MSEC(new_value->it_value.tv_sec) + NSEC_TO_MSEC((new_value->it_value.tv_nsec))));
	#endif


	if(new_value->it_value.tv_sec !=0 || new_value->it_value.tv_nsec !=0 )
	{
		if(xTimerIsTimerActive(xtimer) != pdFALSE)
			xTimerStop(xtimer, 0);

		period = ( SEC_TO_MSEC(new_value->it_value.tv_sec) + NSEC_TO_MSEC((new_value->it_value.tv_nsec)))/portTICK_RATE_MS;

		if ( !period )
			period = portTICK_RATE_MS;

		if( xTimerChangePeriod(xtimer, period, 0) == pdFALSE )
		{
			printf("timerfd_settime: cant set time\n");
			return -1;
		}
	}
	else
	{
		if(xTimerIsTimerActive(xtimer) != pdFALSE)
			xTimerStop(xtimer, 0);
	}

	return 0; /* TBD Saurabh*/
}

int timer_close(int fd)
{
	struct timer *t = get_timer_from_fd(fd);
	TimerHandle_t xtimer = t->id;
        SYS_ARCH_DECL_PROTECT(lev);

	if(t == NULL)
	{
		printf("timer_close: t == NULL assert failed\n");
		return -1;
	}

        if( xTimerDelete( xtimer, 10 ) == pdFALSE)
	{
		printf("%s: cant delete \n", __func__);
		return -1;
	}

	/* protect timers list */
        SYS_ARCH_PROTECT(lev);

	LIST_REMOVE(t, list);
	
        SYS_ARCH_UNPROTECT(lev);

	mem_free(t);

	t = NULL;

	close(fd);
	return 0;
}
