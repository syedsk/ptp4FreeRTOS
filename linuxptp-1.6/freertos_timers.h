#ifndef __FRTOS_TIMERS_H_
#define __FRTOS_TIMERS_H_

int is_timer_fd(int fd);
int timerfd_create(int clockid, int flags);
int timerfd_settime(int fd, int flags,
				  const struct itimerspec *new_value,
				  struct itimerspec *old_value);
int freertos_timers_init();
int timer_close(int fd);
#endif 
