/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* E R R O R   A N D   D I A G N O S T I C   R O U T I N E S */

/* stripped down version from the one in the Modula-2 compiler */
/* $Id$ */

/*	This file contains the error-message and diagnostic
	giving functions.  Be aware that they are called with a variable
	number of arguments!
*/

#include	<varargs.h>
#include	<system.h>
#include	"input.h"
#include	"f_info.h"
#include	"LLlex.h"

/* error classes */
#define	ERROR		1
#define	LEXERROR	3
#define	CRASH		5
#define	FATAL		6

int err_occurred;

extern char *symbol2str();

/*	There are three general error-message functions:
		lexerror()	lexical and pre-processor error messages
		error()		syntactic and semantic error messages
		node_error()	errors in nodes
	The difference lies in the place where the file name and line
	number come from.
	Lexical errors report from the global variables LineNumber and
	FileName, node errors get their information from the
	node, whereas other errors use the information in the token.
*/

/*VARARGS1*/
error(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	_error(ERROR, ap);
	va_end(ap);
}

/*VARARGS1*/
Gerror(va_alist)
	va_dcl
{
	va_list ap;
	char *fn = FileName;

	FileName = 0;
	va_start(ap);
	_error(ERROR, ap);
	va_end(ap);
	FileName = fn;
}

/*VARARGS1*/
lexerror(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	_error(LEXERROR, ap);
	va_end(ap);
}

/*VARARGS1*/
fatal(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	_error(FATAL, ap);
	va_end(ap);
	sys_stop(S_EXIT);
}

/*VARARGS1*/
crash(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	_error(CRASH, ap);
	va_end(ap);
#ifdef DEBUG
	sys_stop(S_ABORT);
#else
	sys_stop(S_EXIT);
#endif
}

_error(class, argv)
	int class;
	va_list argv;
{
	/*	_error attempts to limit the number of error messages
		for a given line to MAXERR_LINE.
	*/
	unsigned int ln = 0;
	register char *remark = 0;
	char *fmt = va_arg(argv, char *);
	
	/*	Since name and number are gathered from different places
		depending on the class, we first collect the relevant
		values and then decide what to print.
	*/
	/* preliminaries */
	switch (class)	{
	case ERROR:
	case LEXERROR:
	case CRASH:
	case FATAL:
		err_occurred = 1;
		break;
	}

	/* the remark */
	switch (class)	{	
	case CRASH:
		remark = "CRASH\007";
		break;
	case FATAL:
		remark = "fatal error --";
		break;
	}
	
	/* the place */
	switch (class)	{	
	case ERROR:
		ln = dot.tk_lineno;
		break;
	case LEXERROR:
	case CRASH:
	case FATAL:
		ln = LineNumber;
		break;
	}
	
	if (FileName) fprint(STDERR, "\"%s\", line %u: ", FileName, ln);

	if (remark) fprint(STDERR, "%s ", remark);

	doprnt(STDERR, fmt, argv);		/* contents of error */
	fprint(STDERR, "\n");
}
