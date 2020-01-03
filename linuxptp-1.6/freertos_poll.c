#include "stdio.h"
#include "pdt.h"
#include "fd.h"
#include "poll.h"
#include "FreeRTOS.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "freertos_timers.h"

int freertos_poll_init(void)
{
	return 0;
}

void freertos_post_event(void)
{
}

int freertos_poll(struct pollfd *fds, unsigned int nfds, int timeout )
{
	int loop;
	int count = 0;
	int fd;
	int maxfd = 0;
	int rc;
	fd_set readfds;
	fd_set writefds;

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);


	for(loop = 0; loop < nfds; loop++)
	{
		fd = fds[loop].fd;

		fds[loop].revents = 0;

		if(fd > 0)
		{
			FD_SET(fd, &readfds);

			if(fds[loop].fd > maxfd)
				maxfd = fd;
		}
	}
	
	//printf("max fd: %d\n", maxfd);
	rc = select(maxfd+1, &readfds, &writefds, NULL, NULL);

	if(rc < 0)
	{
		printf("select failed\n");
		return 0;
	}
	else if( rc == 0)
	{
		return 0;
	}

	/* check for who is ready */
	for(loop = 0; loop < nfds; loop++)
	{
		fd = fds[loop].fd;

		if (fd < 0)
			continue;

		if (FD_ISSET(fd, &readfds))
		{
			if(is_timer_fd(fd))
				lwip_clear_timer_event(fd);

			//printf("event on fd : %d\n", fd);
			fds[loop].revents |= POLLIN;
			count++;
		}
	}
	//printf("poll count %d\n", count);
	return count;
}
