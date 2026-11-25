/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

time_t time(time_t* t)
{
	if (t)
		*t = 0;
	return 0;
}
