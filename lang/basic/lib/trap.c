#include <signal.h>
#include <setjmp.h>

#ifndef NSIG
#define NSIG _NSIG
#endif

/* $Header$ */

/* Trap handling */
int	_trpline;	/* BASIC return label */
jmp_buf	trpbuf;

_trpset(nr)
int nr;
{
	/*debug  printf("trap set to %d\n",nr);*/
	_trpline=nr;
}
void
_trpfatal(i)
int i;
{
	extern int _errsym,_erlsym;

	_errsym= i;
	_setline();
	if( _trpline == 0)
		printf("LINE %d: FATAL ERROR: trap %d\n",_erlsym,i);
#ifdef DEBUG
	printf("trap occurred %d return %d\n",i,_trpline);
#endif
	_trap();
}

_ini_trp()
{
	/* initialize trap routines */
	int i;

	for(i=0;i<NSIG;i++)
		signal(i,_trpfatal);
}


_settrap(nr)
int nr;
{
	_trpline=nr;
}
_trap()
{
	int line;

	if( _trpline==0) exit(-1);
	line=_trpline;
	_trpline=0;		/* should be reset by user */
	_ini_trp();
	longjmp(trpbuf,line);
}
