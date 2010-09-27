/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include "libsys.h"

int remove(const char* path)
{
	return unlink(path);
}
