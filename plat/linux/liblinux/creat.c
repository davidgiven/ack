/* $Source: /cvsroot/tack/Ack/plat/linux386/libsys/creat.c,v $
 * $State: Exp $
 * $Revision: 1.1 $
 */

#include <stdlib.h>
#include <unistd.h>
#include "libsys.h"

int creat(const char* filename, int mode)
{
	return open(filename, O_CREAT|O_WRONLY|O_TRUNC, mode);
}
