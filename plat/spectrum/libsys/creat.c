/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

int open(const char* path, int access, ...)
{
	errno = EACCES;
	return -1;
}
