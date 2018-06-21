/*
 * tzset - set timezone information
 */
/* $Id$ */

/* This function is present for System V && POSIX */

#include <time.h>
#include "loc_time.h"

void tzset(void)
{
	_tzset(); /* does the job */
}
