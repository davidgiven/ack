/*
 * tzset - set timezone information
 */
/* $Header$ */

/* This function is present for System V && POSIX */

#include	<time.h>
#include	"loc_incl.h"

void
tzset(void)
{
	_tzset();	/* does the job */
}
