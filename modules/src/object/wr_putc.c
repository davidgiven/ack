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
