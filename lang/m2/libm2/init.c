/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
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
	/* sigtrp(M2_UNIXSIG, SIGFPE); leave this one for EM runtime startoff*/
	sigtrp(M2_UNIXSIG, SIGBUS);
	sigtrp(M2_UNIXSIG, SIGSEGV);
	sigtrp(EBADMON, SIGSYS);
	sigtrp(M2_UNIXSIG, SIGPIPE);
	sigtrp(M2_UNIXSIG, SIGALRM);
	sigtrp(M2_UNIXSIG, SIGTERM);
}

extern int catch();

int (*handler)() = catch;
char **argv, **environ;
int argc, StackSize;
char *CurrentProcess, MainProcess, StackBase, MainLB;
