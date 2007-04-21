/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define	NEXITS	32

void (*__functab[NEXITS])(void);
int __funccnt = 0;

/* only flush output buffers when necessary */
int (*_clean)(void) = NULL;

static void
_calls(void)
{
	register int i = __funccnt;
	
	/* "Called in reversed order of their registration" */
	while (--i >= 0)
		(*__functab[i])();
}

void
exit(int status)
{
	_calls();
	if (_clean) _clean();
	_exit(status) ;
}
