/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#ifndef _ACK_CONFIG_H
#define _ACK_CONFIG_H

/* We're providing a time() system call rather than wanting a wrapper around
 * gettimeofday() in the libc. */
 
#define ACKCONF_TIME_IS_A_SYSCALL

#endif
