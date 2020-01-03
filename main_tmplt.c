/* template file only not tested */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
/* populate these */
char cmdline_slave[CMD_LEN];
char cmdline_master[CMD_LEN];

/* buffer to store config file data */
char file_buf_slave[1024];
char file_buf_master[1024];

#define ARG_LEN		30
void application_thread(void *ptr)
{
	int i = 0;
	char *ptp_argv[ARG_NUM];
	char *pch;
        int instance = (int) ptr;

	if(instance == PTP_SLAVE)
		pch = strtok (cmdline_slave, " ");
	else
		pch = strtok (cmdline_master, " ");

	while (pch != NULL && i < ARG_NUM) {
		ptp_argv[i] = pch;
		i++;
		pch = strtok (NULL, " ");
	}

	ptp_argv[i] = NULL;

	if(instance == PTP_SLAVE)
	{
	     sys_sem_wait(&mac0_sem);
	     /* ptp_main is the entry into ptp4l */
	     ptp_main(i, ptp_argv, instance, file_buf_slave);
	}
	else
	{
	     sys_sem_wait(&mac1_sem);
	     ptp_main(i, ptp_argv, instance, file_buf_master);
	}

	LPRINTF("r5>ptp exited \n");

	vTaskDelete(NULL);
}

void network_thread(void *p)
{
    if( sys_sem_new(&mac0_sem, 0) != ERR_OK )
    {
	    printf("failed to create sem\n");
    }

    if( sys_sem_new(&mac1_sem, 0) != ERR_OK )
    {
	    printf("failed to create sem\n");
    }

    /* initialize two mac interface */
    mac_init(0, &emac_config[0]);
    mac_init(1, &emac_config[1]);

    sys_sem_signal(&mac0_sem);
    sys_sem_signal(&mac1_sem);

    vTaskDelete(NULL);
}

int app (struct hil_proc *hproc)
{
	/* initialize lwIP before calling sys_thread_new */
	lwip_init();

	freertos_timers_init();
	freertos_poll_init();

	/* any thread using lwIP should be created using sys_thread_new */
	sys_thread_new("NW_THRD", network_thread, NULL,
		THREAD_STACKSIZE,
		PTP_THREAD_PRIO);


	sys_thread_new("ptp4f_M", application_thread, (void *)PTP_SLAVE,
					PTP_THREAD_STACKSIZE,
					PTP_THREAD_PRIO);

	sys_thread_new("ptp4f_S", application_thread, (void *)PTP_MASTER,
					PTP_THREAD_STACKSIZE,
					PTP_THREAD_PRIO);

	while(1) {
		/* put a condition for graceful exit if required*/
	}

}

static void processing(void *unused_arg)
{
	init_system();

	app();

	cleanup_system();
	/* Terminate this task */
	vTaskDelete(NULL);
}

int main()
{
	BaseType_t stat;

	/* Create the tasks */
	stat = xTaskCreate(processing, ( const char * ) "HW2",
				THREAD_STACKSIZE, NULL, 2, &comm_task);
	if (stat != pdPASS) {
		LPERROR("cannot create task\n");
	} else {
		/* Start running FreeRTOS tasks */
		vTaskStartScheduler();
	}

	/* Will not get here, unless a call is made to vTaskEndScheduler() */
	while (1) {
		__asm__("wfi\n\t");
	}

	/* suppress compilation warnings*/
	return 0;
}

