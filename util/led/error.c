#ifndef lint
static char rcsid[] = "$Header$";
#endif

#include <stdio.h>
#include <signal.h>
#include "../../h/out.h"
#include "const.h"

static short	nerrors = 0;

static
stop()
{
	extern char	*outputname;

	if (nerrors)
		unlink(outputname);

	exit(nerrors);
}

trap_signals()
{
	static int	trap_them[] = { SIGHUP, SIGINT, SIGQUIT, SIGTERM, 0 };
	register int	*ip;

	for (ip = trap_them; *ip; ip++)
		if (signal(*ip, stop) == SIG_IGN)
			signal(*ip, SIG_IGN);	/* Oops, reset. */
}

/* VARARGS1 */
fatal(format, a1, a2, a3, a4)
	char	*format;
{
	nerrors++;
	diag("fatal", format, a1, a2, a3, a4);
	stop();
}

/* VARARGS1 */
warning(format, a1, a2, a3, a4)
	char	*format;
{
	diag("warning", format, a1, a2, a3, a4);
}

/* VARARGS1 */
error(format, a1, a2, a3, a4)
	char	*format;
{
	nerrors++;
	diag("error", format, a1, a2, a3, a4);
}

static
diag(tail, format, a1, a2, a3, a4)
	char	*tail;
	char	*format;
{
	extern char	*progname, *archname, *modulname; 

	fprintf(stderr, "%s: ", progname);
	if (archname)
		fprintf(stderr, "%s: ", archname);
	if (modulname)
		fprintf(stderr, "%s: ", modulname);
	fprintf(stderr, format, a1, a2, a3, a4);
	fprintf(stderr, " (%s)\n", tail);
}
