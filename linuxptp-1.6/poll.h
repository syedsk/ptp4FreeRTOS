#define POLLIN		0x0001
#define POLLPRI		0x0002
#define POLLOUT		0x0004
#define POLLERR		0x0008
#define POLLHUP		0x0010
#define POLLNVAL	0x0020

struct pollfd
{
	int fd;
	short events;
	short revents;
};

int freertos_poll(struct pollfd *fds, unsigned int nfds, int timeout );
