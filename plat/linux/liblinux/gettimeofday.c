/* $Source: /cvsroot/tack/Ack/plat/linux386/libsys/gettimeofday.c,v $
 * $State: Exp $
 * $Revision: 1.1 $
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
