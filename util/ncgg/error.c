/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
static char rcsid[]= "$Id$";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "extern.h"

int nerrors=0;

void yyerror(char *s)
{
	error("Parser gives %s",s);
}

void goodbye(void) {

	error("This was fatal, goodbye!");
#ifndef NDEBUG
	abort();
#endif
}

void errorv(const char* s, va_list ap)
{
	extern int lineno;
	extern char *filename;

	fprintf(stderr, "\"%s\", line %d:", filename, lineno);
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");
	nerrors++;
}

void fatal(const char* s, ...)
{

	va_list ap;

	va_start(ap, s);
	errorv(s, ap);
	va_end(ap);

	errorexit();
	goodbye();
	exit(-1);
}

void error(const char* s, ...)
{
	va_list ap;

	va_start(ap, s);
	errorv(s, ap);
	va_end(ap);
}

int tabovf(char *string)
{
	fatal("%s overflow",string);
	return 0;
}
