/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/*
  Module:	initialization and some global vars
  Author:	Ceriel J.H. Jacobs
  Version:	$Header$
*/

#include <signal.h>
#include <em_abs.h>
#include <m2_traps.h>

/* map unix signals onto EM traps */
init()
{
	sigtrp(M2_UNIXSIG, SIGHUP);
	sigtrp(M2_UNIXSIG, SIGINT);
	sigtrp(M2_UNIXSIG, SIGQUIT);
	sigtrp(EILLINS, SIGILL);
	sigtrp(M2_UNIXSIG, SIGTRAP);
	sigtrp(M2_UNIXSIG, SIGIOT);
	sigtrp(M2_UNIXSIG, SIGEMT);
	sigtrp(M2_UNIXSIG, SIGFPE);
	sigtrp(M2_UNIXSIG, SIGBUS);
	sigtrp(M2_UNIXSIG, SIGSEGV);
	sigtrp(EBADMON, SIGSYS);
	sigtrp(M2_UNIXSIG, SIGPIPE);
	sigtrp(M2_UNIXSIG, SIGALRM);
	sigtrp(M2_UNIXSIG, SIGTERM);
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
char **argv = 0, **environ = 0;
int argc = 0;
char *MainLB = 0;
