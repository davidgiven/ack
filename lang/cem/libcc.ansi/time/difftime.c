/*
 * difftime - compute the difference between two calendar times
 */
/* $Header$ */

#include	<time.h>

double
difftime(time_t time1, time_t time0)
{
	if (time0 > time1)
		return - (double) (time0 - time1);
	else	return (double) (time1 - time0);
}
