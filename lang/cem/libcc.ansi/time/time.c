/*
 * time - return the current calendar time (seconds since jan 1, 1970)
 */
/* $Header$ */

#include	<time.h>
#include	<sys/time.h>

time_t
time(time_t *timer)
{
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);

	if (timer) *timer = tv.tv_sec;
	return tv.tv_sec;
}
