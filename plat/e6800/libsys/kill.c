/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

int kill(pid_t pid, int sig)
{
	errno = EINVAL;
	return -1;
}
