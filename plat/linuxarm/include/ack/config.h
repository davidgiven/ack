/* $Source: /cvsroot/tack/Ack/plat/linux386/include/ack/config.h,v $
 * $State: Exp $
 * $Revision: 1.1 $
 */

#ifndef _ACK_CONFIG_H
#define _ACK_CONFIG_H

/* We're providing a time() system call rather than wanting a wrapper around
 * gettimeofday() in the libc. */
 
/* #define ACKCONF_TIME_IS_A_SYSCALL */

/* We don't support floating point right now. */
 
#define ACKCONF_NO_STDIO_FLOAT

#endif
