/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdio.h>

static char buf[BUFSIZ];

extern char *_pbuf;
extern int _ocnt;

wr_putc(ch)
{
	_pbuf = buf;
	buf[_ocnt++] = ch;
	if (_ocnt == BUFSIZ) {
		wr_flush();
	}
}
