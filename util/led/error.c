/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <out.h>
#include "const.h"

static short	nerrors = 0;
static void	diag(char *, char *, va_list);

stop()
{
	extern char	*outputname;
	extern int	exitstatus;

	if (nerrors) {
		unlink(outputname);
		exit(nerrors);
	}

	exit(exitstatus);
}

/* VARARGS1 */
void
fatal(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	nerrors++;
	diag("fatal", format, ap);
	stop();
}

/* VARARGS1 */
void
warning(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	diag("warning", format, ap);
	va_end(ap);
}

/* VARARGS1 */
void
error(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	nerrors++;
	diag("error", format, ap);
	va_end(ap);
}

/* VARARGS1 */
void
do_verbose(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	diag((char *) 0, format, ap);
	va_end(ap);
}

static void
diag(char *tail, char *format, va_list ap)
{
	extern char	*progname, *archname, *modulname;

	fprintf(stderr, "%s: ", progname);
	if (archname && modulname)
		fprintf(stderr, "%s(%.14s): ", archname, modulname);
	else if (archname)
		fprintf(stderr, "%s: ", archname);
	else if (modulname)
		fprintf(stderr, "%s: ", modulname);
	vfprintf(stderr, format, ap);
	if (tail) fprintf(stderr, " (%s)\n", tail);
	else putc('\n', stderr);
}
