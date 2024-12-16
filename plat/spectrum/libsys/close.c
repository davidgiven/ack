/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

int close(int fd)
{
	errno = EBADF;
	return -1;
}
