#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

#ifndef NSIG
#define NSIG _NSIG
#endif

/* $Id$ */

/* Trap handling */
int _trpline; /* BASIC return label */
jmp_buf trpbuf;

void _trpset(int nr)
{
	/*debug  printf("trap set to %d\n",nr);*/
	_trpline = nr;
}
void _trpfatal(int i)
{
	extern int _errsym, _erlsym;

	_errsym = i;
	_setline();
	if (_trpline == 0)
		printf("LINE %d: FATAL ERROR: trap %d\n", _erlsym, i);
#ifdef DEBUG
	printf("trap occurred %d return %d\n", i, _trpline);
#endif
	_trap();
}

void _ini_trp(void)
{
	/* initialize trap routines */
	int i;

	for (i = 0; i < NSIG; i++)
		signal(i, _trpfatal);
}

void _settrap(int nr)
{
	_trpline = nr;
}
void _trap(void)
{
	int line;

	if (_trpline == 0)
		exit(-1);
	line = _trpline;
	_trpline = 0; /* should be reset by user */
	_ini_trp();
	longjmp(trpbuf, line);
}
