/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#ifndef _TERMIOS_H
#define _TERMIOS_H

typedef unsigned char tcflag_t;

struct termios
{
    tcflag_t c_iflag;
    tcflag_t c_oflag;
    tcflag_t c_lflag;
};

#define ONLCR 1
#define ECHO 2
#define INLCR 4

#define TCSANOW 0
#define TCSADRAIN 1
#define TCSAFLUSH 2

extern int tcgetattr(int fd, struct termios* t);
extern int tcsetattr(int fd, int actions, struct termios* t);

#endif
