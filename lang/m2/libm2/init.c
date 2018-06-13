/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/*
  Module:	initialization and some global vars
  Author:	Ceriel J.H. Jacobs
  Version:	$Id$
*/

#include <signal.h>
#include <em_abs.h>
#include <m2_traps.h>

static const char signals_list[] = {
#ifdef SIGHUP
	SIGHUP,
#endif
#ifdef SIGINT
	SIGINT,
#endif
#ifdef SIGQUIT
	SIGQUIT,
#endif
#ifdef SIGTRAP
	SIGTRAP,
#endif
#ifdef SIGIOT
	SIGIOT,
#endif
#ifdef SIGEMT
	SIGEMT,
#endif
#ifdef SIGFPE
	SIGFPE,
#endif
#ifdef SIGBUS
	SIGBUS,
#endif
#ifdef SIGSEGV
	SIGSEGV,
#endif
#ifdef SIGPIPE
	SIGPIPE,
#endif
#ifdef SIGALRM
	SIGALRM,
#endif
#ifdef SIGTERM
	SIGTERM,
#endif
	-1
};

/* map unix signals onto EM traps */
void init(void)
{
	const char* p = signals_list;
	do {
		int i = *p++;
		if (i == -1)
			break;
		sigtrp(M2_UNIXSIG, i);
	} while (1);

	sigtrp(EILLINS, SIGILL);
#ifdef SIGSYS
	sigtrp(EBADMON, SIGSYS);
#endif
}
#if defined(__em22) || defined(__em24) || defined(__em44)
killbss()
{
}
#else

static int blablabla;		/*	We cannot use end, because then also
					bss allocated for the systemcall lib
					would be overwritten. Lets hope that
					this helps ...
				*/

killbss()
{
	extern char *bkillbss;
	register char *p = (char *) &bkillbss;

	while (p < (char *) &blablabla) *p++ = 0x66;
}
#endif

extern int catch();

int (*handler)() = catch;
char **argv = 0;
int argc = 0;
char *MainLB = 0;
