/* $Id$ */
#include <sys/types.h>
#include <sys/timeb.h>
time_t
time(timpt) time_t *timpt ; {
	struct timeb buf ;

	ftime(&buf) ;
	if ( timpt ) *timpt= buf.time ;
	return buf.time ;
}
