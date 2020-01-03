
#include <stdio.h>
#include "netif/etharp.h"
#include "lwip/stats.h"

//#define DEBUG

#define PTP_RX_THREAD_PRIO  ( configMAX_PRIORITIES - 1 )
#define THREAD_STACKSIZE 1024

#ifdef OS_IS_FREERTOS
unsigned int xInsideISR = 0;
#endif

static void emac_drv_set_ts(struct netif *netif, struct pbuf *p)
{
	struct emac_drv_local *cb = (struct emac_drv_local *) netif->state;

 
	/* set ts  from hw */
	p->ts_sec = captured_sec;
	p->ts_nsec = captured_ns;

}

static void emac_drv_recv(struct netif *netif)
{
	emac_drv_set_ts(netif, p);

	if( netif->input(p, netif) != ERR_OK )
	{
		pbuf_free(p);
		p = NULL;
	}
}
#if !NO_SYS
/*
 * The input thread calls lwIP to process any received packets.
 * This thread waits until a packet is received (sem_rx),
 * and then calls xemacif_input which processes 1 packet at a time.
 */
void
emac_drv_local_input_thread(struct netif *netif)
{
	struct emac_drv_local *cb = (struct emac_drv_local *) netif->state;

	while (1) {
		/* sleep until there are packets to process
		 * This semaphore is set by the packet receive interrupt
		 * routine.
		 */
		sys_sem_wait(&cb->sem_rx);

		emac_drv_recv(netif);
	}
}
#endif




/* Define those to better describe your network interface. */
#define IFNAME0 't'
#define IFNAME1 'e'

static err_t low_level_init(struct netif *netif)
{
	struct emac_drv_local *cb;
	struct emac_config *config = (struct emac_config *) netif->state;

	int tx_irq, rx_irq;

	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP |
					NETIF_FLAG_LINK_UP;

	cb = mem_malloc(sizeof(struct emac_drv_local));
	if(cb == NULL)
	{
		printf("%s: malloc failed\n", __func__);
		return ERR_MEM;
	}

	tx_irq = config->ptp_tx_irq;
	rx_irq = config->ptp_rx_irq;

#if !NO_SYS
	sys_sem_new(&cb->sem_rx, 0);
	sys_sem_new(&cb->sem_tx, 0);
#endif

	cb->base = config->base;


	netif->state = (void *)cb;

	/* start packet receive thread - required for lwIP operation */
	sys_thread_new("emac_drv_local_input_thread", (void(*)(void*))emac_drv_local_input_thread, netif,
            THREAD_STACKSIZE,
            PTP_RX_THREAD_PRIO);

	return ERR_OK;
}



static err_t emac_drv_xmit(struct netif *netif, struct pbuf *p)
{
	/* your emac driver xmit */
	emac_drv_set_ts(netif, p);
}
/*
 * low_level_output():
 *
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 */
static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
	int err;
        SYS_ARCH_DECL_PROTECT(lev);

	#ifdef DEBUG
	printf("%s <---\n", __func__);
	#endif
	struct pbuf *q;

        SYS_ARCH_PROTECT(lev);

	for(q = p; q!=NULL; q = q->next)
	{
		err = emac_drv_xmit(netif, q);
		if(err != ERR_OK)
			break;
	}

        SYS_ARCH_UNPROTECT(lev);

	return ERR_OK;
}


err_t
emac_drv_init(struct netif *netif)
{
	netif->name[0] = IFNAME0;
	netif->name[1] = IFNAME1;
	netif->output = NULL; /* TODO */
	netif->linkoutput = low_level_output;

	low_level_init(netif);

	return ERR_OK;
}
