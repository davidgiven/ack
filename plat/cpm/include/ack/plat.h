/* $Source$
 * $State$
 * $Revision$
 */

#ifndef _ACK_PLAT_H
#define _ACK_PLAT_H

/* We're providing a time() system call rather than wanting a wrapper around
 * gettimeofday() in the libc. */
 
#define ACKCONF_WANT_EMULATED_TIME 0

/* CP/M's underlying file abstraction is weird and doesn't map well onto
 * file descriptors. Disable the standard FILE* mechanism in favour of our
 * own.
 */

#define ACKCONF_WANT_EMULATED_FILE 0
#define ACKCONF_WANT_EMULATED_POPEN 0

#endif
