#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "address.h"
#include "contain.h"
#include "print.h"
#include "transport_private.h"
#include "uds.h"



struct uds {
	struct transport t;
	struct address address;
};

static int uds_close(struct transport *t, struct fdarray *fda)
{
	struct uds *uds = container_of(t, struct uds, t);
#if 0
	uds_ready = FALSE;

	if(uds->mbox != SYS_MBOX_NULL)
		sys_mbox_free(&uds->mbox);

	close(uds_fd);
#endif
	printf("uds_close\n");
	return 0;
}

static int uds_send(struct transport *t, struct fdarray *fda, int event,
		    int peer, void *buf, int buflen, struct address *addr,
		    struct hw_timestamp *hwts)
{
	struct uds *uds = container_of(t, struct uds, t);

	printf("uds_send\n");
	return -1;
}

static int uds_recv(struct transport *t, int fd, void *buf, int buflen,
		    struct address *addr, struct hw_timestamp *hwts)
{
	struct uds *uds = container_of(t, struct uds, t);

	printf("uds_recv\n");
	
	return -1;
}

static int uds_open(struct transport *t, const char *name, struct fdarray *fda,
		    enum timestamp_type tt)
{
	struct uds *uds = container_of(t, struct uds, t);
	printf("uds_open\n");

	fda->fd[FD_EVENT] = -1;
	fda->fd[FD_GENERAL] = -1;

	return 0;
}

static void uds_release(struct transport *t)
{
	struct uds *uds = container_of(t, struct uds, t);
	printf("uds_release\n");
	mem_free(uds);
}

struct transport *uds_transport_create(void)
{
	struct uds *uds;
	uds = mem_calloc(1, sizeof(*uds));
	if (!uds)
		return NULL;
	uds->t.close   = uds_close;
	uds->t.open    = uds_open;
	uds->t.recv    = uds_recv;
	uds->t.send    = uds_send;
	uds->t.release = uds_release;
	return &uds->t;
}
