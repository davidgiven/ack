/*
 * time - return the current calendar time (seconds since jan 1, 1970)
 */
/* $Id$ */

#if	defined(__BSD4_2)
#include	<time.h>
/*
 * Structure returned by gettimeofday(2) system call,
 * and used in other calls.
 */
struct timeval {
	long	tv_sec;		/* seconds */
	long	tv_usec;	/* and microseconds */
};

struct timezone {
	int	tz_minuteswest;	/* minutes west of Greenwich */
	int	tz_dsttime;	/* type of dst correction */
};

int _gettimeofday(struct timeval *tp, struct timezone *tzp);

time_t
time(time_t *timer)
{
	struct timeval tv;
	struct timezone tz;
	_gettimeofday(&tv, &tz);

	if (timer) *timer = tv.tv_sec;
	return tv.tv_sec;
}
#else
/* Assume time() is a system call */	/* ??? */
#endif
