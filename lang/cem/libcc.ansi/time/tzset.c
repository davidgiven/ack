/*
 * tzset - system V compatibility
 */
/* $Header$ */

#ifdef	__USG

#include	<time.h>
#include	"loc_incl.h"

void
tzset(void)
{
	_tzone();	/* does the job */
}
#endif	/* __USG */
