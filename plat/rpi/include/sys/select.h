/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H

#include <stdint.h>

typedef uint32_t fd_set;

extern int select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout);

#define FD_ZERO(set) do { *set = 0; } while (0)
#define FD_SET(fd, set) do { *set |= (1<<fd); } while (0);
#define FD_CLR(fd, set) do { *set &= ~(1<<fd); } while (0);
#define FD_ISSET(fd, set) (*set | (1<<fd))

#endif
