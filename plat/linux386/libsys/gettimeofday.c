/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "libsys.h"

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	return _syscall(__NR_gettimeofday, (quad) tv, (quad) tz, 0);
}
