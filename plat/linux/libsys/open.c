/* $Source: /cvsroot/tack/Ack/plat/linux386/libsys/open.c,v $
 * $State: Exp $
 * $Revision: 1.1 $
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include "libsys.h"

int open(const char* path, int access, ...)
{
	int mode = 0;
	
	if (access & O_CREAT)
	{
		va_list ap;
		va_start(ap, access);
		mode = va_arg(ap, int);
		va_end(ap);
	}
	
	return _syscall(__NR_open, (quad) path, access, mode);
}
