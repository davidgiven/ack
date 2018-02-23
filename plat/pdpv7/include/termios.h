/*
 * Raspberry Pi support library for the PDP/11
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
    tcflag_t c_cflag;
};

#define ONLCR 1
#define ECHO 2
#define INLCR 4

/* Dummied parameters for compatibility --- only the ones above are
 * honoured. */

#define BRKINT 0
#define ICRNL 0
#define INPCK 0
#define ISTRIP 0
#define IXON 0
#define CS8 0
#define ICANON 0
#define IEXTEN 0
#define ISIG 0

#define OPOST ONLCR

#define TCSANOW 0
#define TCSADRAIN 1
#define TCSAFLUSH 2

extern int tcgetattr(int fd, struct termios* t);
extern int tcsetattr(int fd, int actions, struct termios* t);

#endif
