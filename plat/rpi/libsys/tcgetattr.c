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

int tcgetattr(int fd, struct termios* t)
{
    t->c_iflag = _sys_ttyflags & INLCR;
    t->c_oflag = _sys_ttyflags & ONLCR;
    t->c_lflag = _sys_ttyflags & ECHO;
    t->c_cflag = 0;
    return 0;
}

