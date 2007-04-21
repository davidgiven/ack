/*
 * time - return the current calendar time (seconds since jan 1, 1970)
 */
/* $Id$ */

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <ack/config.h>

#ifndef ACKCONF_TIME_IS_A_SYSCALL

time_t
time(time_t *timer)
{
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);

	if (timer) *timer = tv.tv_sec;
	return tv.tv_sec;
}

#endif
