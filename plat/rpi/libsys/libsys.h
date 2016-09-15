/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#ifndef LIBSYS_H
#define LIBSYS_H

extern void _sys_rawwrite(unsigned char b);
extern unsigned char _sys_rawread(void);
extern int _sys_rawpoll(void);

extern void _sys_write_tty(char c);

extern int _sys_ttyflags;

#endif
