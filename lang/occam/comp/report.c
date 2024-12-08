/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include <stdlib.h>
#include <system.h>

extern int err, lineno;
extern char *curr_file;

/*VARARGS1*/
report(fmt, arg1, arg2, arg3) char *fmt;
{
	fprintf(stderr, "%s (%d) F: ", curr_file, lineno);
	fprintf(stderr, fmt, arg1, arg2, arg3);
	fprintf(stderr,"\n");
	err=1;
}

/*VARARGS1*/
warning(fmt, arg1, arg2, arg3) char *fmt, *arg1;
{
	fprintf(stderr, "%s (%d) E: ", curr_file, lineno);
	fprintf(stderr, fmt, arg1, arg2, arg3);
	fprintf(stderr,"\n");
}

/*VARARGS1*/
fatal(fmt, arg1, arg2, arg3) char *fmt, *arg1;
{
	fprintf(stderr, "%s (%d) X: ", curr_file, lineno);
	fprintf(stderr, fmt, arg1, arg2, arg3);
	fprintf(stderr,"\n");
	exit(1);
}
