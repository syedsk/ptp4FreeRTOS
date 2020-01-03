#ifndef TIMER_1588_H
#define TIMER_1588_H

#define TEMAC1_BASEADDR			0x80040000 /* TEMAC1 */
#define TEMAC2_BASEADDR			0x80060000 /* TEMAC1 */
/* RTC Increment */
#define XTIMER1588_RTC_INCREMENT        0x12810
/* RTC Nanoseconds Field Offset Register */
#define XTIMER1588_RTC_OFFSET_NS        0x12800
/* RTC Seconds Field Offset Register - Low */
#define XTIMER1588_RTC_OFFSET_SEC_L     0x12808
/* RTC Seconds Field Offset Register - High */
#define XTIMER1588_RTC_OFFSET_SEC_H     0x1280C

int ptp_adjfreq(int id, s32 ppb);
int ptp_adjtime(int id, s64 delta);

#endif
