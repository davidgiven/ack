/*
 * ctime - convers the calendar time to a string
 */
/* $Id$ */

#include <time.h>

char*(ctime)(const time_t* timer)
{
	return asctime(localtime(timer));
}
