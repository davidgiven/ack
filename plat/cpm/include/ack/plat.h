/* $Source$
 * $State$
 * $Revision$
 */

#ifndef _ACK_PLAT_H
#define _ACK_PLAT_H

/* We're providing a time() system call rather than wanting a wrapper around
 * gettimeofday() in the libc. */
 
#define ACKCONF_TIME_IS_A_SYSCALL

#endif
