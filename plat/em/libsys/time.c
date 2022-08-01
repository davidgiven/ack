/* $Id$ */
#include <sys/types.h>
#include <sys/timeb.h>

extern void ftime(struct timeb* buf);

time_t time(time_t* timpt) {
	struct timeb buf ;

	ftime(&buf) ;
	if ( timpt ) *timpt= buf.time ;
	return buf.time ;
}
