/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include "libsys.h"

int unlink(const char* path)
{
	return _syscall(__NR_unlink, (quad) path, 0, 0);
}
