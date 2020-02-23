# ptp4FreeRTOS
FreeRTOS port of linuxptp(ptp4l). 
Ideally suited to run PTP4L on a dedicated core (say cortex-R5). 

# Features
1. Supports linuxptp v1.6
2. Supports multi instance (currently tested with two ptp4l instances)
3. Uses lwip-1.4.1
4. Changes are needed in LWIP to support timestamping. Included in this repo as a LWIP patch.
5. Tested with gPTP profile only
6. HW Tested with:
    Xilinx TSN IP reference design on zcu102 board. FreeRTOS running on R5. 

# Building
   See the template makefile. You need to cross-compile for your target arch.
   Link LWiP(with the patches) and freertos libraries to generate freertos elf 
   There is a template emac_driver (which registers two mac netif)

# Porting Guide
## 1. clock_adjtime
     clock_adjtime is implemented in missing.h
     clock_adjtime calls functions from timer_1588.c(ptp_adjfreq/ptp_adjtime)
    These are currenly implemented as direct adjustment of RTC clock of Xilnx TSN HW
    This must be changed for the target HW/RTC.
## 2. HW Timestamping Support 
    Two new APIs lwip_send_with_ts and lwip_recv_with_ts to support timestamping.
    See lwip-1.4.1/src/api/sockets.c
    lwip-1.4.1/src/include/lwip/pbuf.h/struct pbuf has two new elements:
      u32_t ts_sec;
      u32_t ts_nsec;
    These values need to be set in your lwip emac driver implementation:
    while doing RX via netif->input()
    while doing TX via low_level_output() 
## 3. Polling mechanism with Lwip
    ptp4l uses poll() system call to wait for timer events as well as network events. 
    Lwip has polling only for socket fd. The timers in freertos are implemented as 
    psuedo socket so same select() call can be used for network & timer events
    
    See linuxptp-1.6/freertos_poll.c/freertos_poll()
    See lwip-1.4.1/src/api/sockets.c/lwip_post_timer_event() and
        linuxptp-1.6/freertos_timers.c/timer_callback()
 

    
