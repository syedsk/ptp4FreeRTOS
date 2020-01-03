# ptp4FreeRTOS
FreeRTOS port of linuxptp(ptp4l)
# Features
1. Supports linuxptp v1.6
2. Supports multi instance (currently tested with two ptp4l instances)
3. Uses lwip-1.4.1
4. Changes are needed in LWIP to support timestamping. Included in this repo as a LWIP patch.
5. Tested with gPTP profile only

# Porting Guide
## 1. clock_adjtime is implemented in missing.h
## 2. clock_adjtime calls functions from timer_1588.c(ptp_adjfreq/ptp_adjtime)
    These are currenly implemented as direct adjustment of RTC clock of Xilnx TSN HW
    This must be changed for the target HW/RTC
## 3. Two new APIs lwip_send_with_ts and lwip_recv_with_ts to support timestamping.
## 4. src/include/lwip/pbuf.h/struct pbuf has two new elements:
    u32_t ts_sec;
    u32_t ts_nsec;
    These values need to be set in your lwip emac driver implementation:
    while doing RX via netif->input()
    while doing TX via low_level_output() 
## 5. Polling mechanism with Lwip
    ptp4l uses poll to wait for timer events as well as network events. 
    Lwip has polling only for socket fd. The timers in freertos are implemented as 
    sudo socket so same select() call can be used for network/timer events
    
    See linuxptp-1.6/freertos_poll.c/freertos_poll()
    See lwip-1.4.1/src/api/sockets.c/lwip_post_timer_event() and linuxptp-1.6/freertos_timers.c/timer_callback()
 

    
