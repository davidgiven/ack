/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	E R R O R   A N D  D I A G N O S T I C   R O U T I N E S	*/

#include	<system.h>
#if __STDC__
#include	<stdarg.h>
#else
#include	<varargs.h>
#endif

#include    "parameters.h"
#include	"arith.h"


#include	"LLlex.h"

/*	This file contains the (non-portable) error-message and diagnostic
	functions.  Beware, they are called with a variable number of
	arguments!
*/

int err_occurred;

static void err_hdr(char *s)
{
	if (FileName) {
		fprintf(ERROUT, "\"%s\", line %d: %s", FileName, (int)LineNumber, s);
	}
	else	fprintf(ERROUT, s);
}

#if __STDC__
/*VARARGS*/
void error(char *fmt, ...)
{
	va_list ap;

	err_occurred = 1;
	err_hdr("");
	va_start(ap, fmt);
	vfprintf(ERROUT, fmt, ap);
	fprintf(ERROUT, "\n");
	va_end(ap);
}

/*VARARGS*/
void warning(char *fmt, ...)
{
	va_list ap;

	err_hdr("(warning) ");
	va_start(ap, fmt);
	vfprintf(ERROUT, fmt, ap);
	fprintf(ERROUT, "\n");
	va_end(ap);
}

/*VARARGS*/
void strict(char *fmt, ...)
{
	va_list ap;

	err_hdr("(strict) ");
	va_start(ap, fmt);
	vfprintf(ERROUT, fmt, ap);
	fprintf(ERROUT, "\n");
	va_end(ap);
}

/*VARARGS*/
NORETURN void crash(char *fmt, ...)
{
	va_list ap;

	err_hdr("CRASH\007 ");
	va_start(ap, fmt);
	vfprintf(ERROUT, fmt, ap);
	fprintf(ERROUT, "\n");
	va_end(ap);
	abort();
}

/*VARARGS*/
NORETURN void fatal(char *fmt, ...)
{
	va_list ap;

	err_hdr("fatal error -- ");
	va_start(ap, fmt);
	vfprintf(ERROUT, fmt, ap);
	fprintf(ERROUT, "\n");
	va_end(ap);
	exit(1);
}
#else
/*VARARGS*/
void error(va_alist)
	va_dcl
{
	char *fmt;
	va_list ap;

	err_occurred = 1;
	err_hdr("");
	va_start(ap);
	fmt = va_arg(ap, char *);
	vfprintf(ERROUT, fmt, ap);
	fprintf(ERROUT, "\n");
	va_end(ap);
}

/*VARARGS*/
void warning(va_alist)
	va_dcl
{
	char *fmt;
	va_list ap;

	err_hdr("(warning) ");
	va_start(ap);
	fmt = va_arg(ap, char *);
	vfprintf(ERROUT, fmt, ap);
	fprintf(ERROUT, "\n");
	va_end(ap);
}

/*VARARGS*/
void strict(va_alist)
	va_dcl
{
	char *fmt;
	va_list ap;

	err_hdr("(strict) ");
	va_start(ap);
	fmt = va_arg(ap, char *);
	vfprintf(ERROUT, fmt, ap);
	fprintf(ERROUT, "\n");
	va_end(ap);
}

/*VARARGS*/
void crash(va_alist)
	va_dcl
{
	char *fmt;
	va_list ap;

	err_hdr("CRASH\007 ");
	va_start(ap);
	fmt = va_arg(ap, char *);
	vfprintf(ERROUT, fmt, ap);
	fprintf(ERROUT, "\n");
	va_end(ap);
	abort();
}

/*VARARGS*/
void fatal(va_alist)
	va_dcl
{
	char *fmt;
	va_list ap;

	err_hdr("fatal error -- ");
	va_start(ap);
	fmt = va_arg(ap, char *);
	vfprintf(ERROUT, fmt, ap);
	fprintf(ERROUT, "\n");
	va_end(ap);
	exit(1);
}
#endif
