/*
 * isatty - check if a file descriptor is associated with a terminal
 */
/* $Id$ */

#include <stdlib.h>

#if ACKCONF_WANT_TERMIOS

#include <termios.h>
int isatty(int fd)
{
  struct termios dummy;

  return(tcgetattr(fd, &dummy) == 0);
}

#endif
