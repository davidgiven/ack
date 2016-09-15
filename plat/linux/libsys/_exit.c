/* $Source: /cvsroot/tack/Ack/plat/linux386/libsys/_exit.c,v $
 * $State: Exp $
 * $Revision: 1.1 $
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

void _exit(int status)
{
	_syscall(__NR_exit, status, 0, 0);
}
