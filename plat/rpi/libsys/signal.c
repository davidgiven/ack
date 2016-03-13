/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include "libsys.h"

sighandler_t signal(int signum, sighandler_t handler)
{
	return SIG_DFL;
}
