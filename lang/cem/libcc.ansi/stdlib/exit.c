/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<stdlib.h>

#define	NEXITS	32

static void (*functab[NEXITS])(void);
static int funccnt = 0;

extern void _cleanup(void);
extern void _exit(int);

static void
_calls(void)
{
	register int i = funccnt;
	
	/* "Called in reversed order of their registration" */
	while (--i >= 0)
		(*functab[i])();
}

void
exit(int status)
{
	_calls();
	_cleanup() ;
	_exit(status) ;
}

int
atexit(void (*func)(void))
{
	if (funccnt >= NEXITS)
		return 1;
	functab[funccnt++] = func;
	return 0;
}
