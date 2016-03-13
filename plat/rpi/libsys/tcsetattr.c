/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include "libsys.h"

int tcsetattr(int fd, int actions, struct termios* t)
{
	_sys_ttyflags = t->c_iflag | t->c_oflag | t->c_lflag;
    return 0;
}

