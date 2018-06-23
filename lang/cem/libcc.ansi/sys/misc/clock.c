/*
 * clock - determine the processor time used
 */
/* $Id$ */

#include <time.h>
#include <sys/times.h>

clock_t
clock(void)
{
	struct tms tms;

	times(&tms);
	/* Assume that time_t can be converted to clock_t for Sys5 */
	return tms.tms_utime;
}
