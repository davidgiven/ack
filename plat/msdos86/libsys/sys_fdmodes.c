/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include "libsys.h"

/*
 * By default, consider all file descriptors, including those for stdin (0),
 * stdout (1), and stderr (2), as being open in text mode.
 */
struct _fdmodes _sys_fdmodes = { NULL, 0, 0 };
