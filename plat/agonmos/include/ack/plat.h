#ifndef _ACK_PLAT_H
#define _ACK_PLAT_H

/* The ez80 code generator doesn't do floating point. */

#define ACKCONF_WANT_STDIO_FLOAT 0

/* We're providing a time() system call rather than wanting a wrapper around
 * gettimeofday() in the libc. */
 
#define ACKCONF_WANT_EMULATED_TIME 0

/* Processes? MOS? Hahahaha... */

#define ACKCONF_WANT_EMULATED_POPEN 0

/* The eZ80 port doesn't do bounds changing. */

#define ACKCONF_HAS_BOUNDS_CHECKING 0

#endif
