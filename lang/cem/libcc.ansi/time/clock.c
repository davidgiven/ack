/*
 * clock - determine the processor time used
 */
/* $Header$ */

#include	<time.h>
#include	<sys/time.h>
#include	<sys/resource.h>

clock_t
clock(void)
{
	struct rusage rusage;

	getrusage(RUSAGE_SELF, &rusage);

	return (((unsigned long)rusage.ru_utime.tv_sec * CLK_TCK)
		    + rusage.ru_utime.tv_usec);
}
