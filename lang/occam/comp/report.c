/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <system.h>

extern int err, lineno;
extern char *curr_file;

/*VARARGS1*/
report(fmt, arg1, arg2, arg3) char *fmt;
{
	fprint(STDERR, "%s (%d) F: ", curr_file, lineno);
	fprint(STDERR, fmt, arg1, arg2, arg3);
	fprint(STDERR,"\n");
	err=1;
}

/*VARARGS1*/
warning(fmt, arg1, arg2, arg3) char *fmt, *arg1;
{
	fprint(STDERR, "%s (%d) E: ", curr_file, lineno);
	fprint(STDERR, fmt, arg1, arg2, arg3);
	fprint(STDERR,"\n");
}

/*VARARGS1*/
fatal(fmt, arg1, arg2, arg3) char *fmt, *arg1;
{
	fprint(STDERR, "%s (%d) X: ", curr_file, lineno);
	fprint(STDERR, fmt, arg1, arg2, arg3);
	fprint(STDERR,"\n");
	exit(1);
}
