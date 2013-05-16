/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

int creat(const char* path, int mode)
{
	return open(path, O_CREAT|O_WRONLY|O_TRUNC, mode);
}
