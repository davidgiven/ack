/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	E R R O R   A N D  D I A G N O S T I C   R O U T I N E S	*/

#include	<system.h>
#if __STDC__
#include	<stdarg.h>
#else
#include	<varargs.h>
#endif

#include	"errout.h"
#include	"LLlex.h"

/*	This file contains the (non-portable) error-message and diagnostic
	functions.  Beware, they are called with a variable number of
	arguments!
*/

int err_occurred;

err_hdr(s)
	char *s;
{
	if (FileName) {
		fprint(ERROUT, "\"%s\", line %d: %s", FileName, LineNumber, s);
	}
	else	fprint(ERROUT, s);
}

#if __STDC__
/*VARARGS1*/
error(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_hdr("");
	err_occurred = 1;
	doprnt(ERROUT, fmt, ap);
	fprint(ERROUT, "\n");
	va_end(ap);
}

/*VARARGS1*/
warning(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_hdr("(warning) ");
	doprnt(ERROUT, fmt, ap);
	fprint(ERROUT, "\n");
	va_end(ap);
}

/*VARARGS1*/
crash(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_hdr("CRASH\007 ");
	doprnt(ERROUT, fmt, ap);
	fprint(ERROUT, "\n");
	va_end(ap);
	sys_stop(S_ABORT);
}

/*VARARGS1*/
fatal(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_hdr("fatal error -- ");
	doprnt(ERROUT, fmt, ap);
	fprint(ERROUT, "\n");
	va_end(ap);
	sys_stop(S_EXIT);
}
#else /* __STDC__ */
/*VARARGS1*/
error(va_alist)
	va_dcl
{
	char *fmt;
	va_list ap;

	err_hdr("");
	err_occurred = 1;
	va_start(ap);
	fmt = va_arg(ap, char *);
	doprnt(ERROUT, fmt, ap);
	fprint(ERROUT, "\n");
	va_end(ap);
}

/*VARARGS1*/
warning(va_alist)
	va_dcl
{
	char *fmt;
	va_list ap;

	err_hdr("(warning) ");
	va_start(ap);
	fmt = va_arg(ap, char *);
	doprnt(ERROUT, fmt, ap);
	fprint(ERROUT, "\n");
	va_end(ap);
}

/*VARARGS1*/
crash(va_alist)
	va_dcl
{
	char *fmt;
	va_list ap;

	err_hdr("CRASH\007 ");
	va_start(ap);
	fmt = va_arg(ap, char *);
	doprnt(ERROUT, fmt, ap);
	fprint(ERROUT, "\n");
	va_end(ap);
	sys_stop(S_ABORT);
}

/*VARARGS1*/
fatal(va_alist)
	va_dcl
{
	char *fmt;
	va_list ap;

	err_hdr("fatal error -- ");
	va_start(ap);
	fmt = va_arg(ap, char *);
	doprnt(ERROUT, fmt, ap);
	fprint(ERROUT, "\n");
	va_end(ap);
	sys_stop(S_EXIT);
}
#endif
