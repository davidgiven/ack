/* $Header$ */
/*	E R R O R   A N D  D I A G N O S T I C   R O U T I N E S	*/

#include	<system.h>
#include	<em.h>

#include	"nopp.h"
#include	"use_tmp.h"
#include	"errout.h"
#include	"debug.h"

#include	"tokenname.h"
#include	"arith.h"
#include	"label.h"
#include	"expr.h"
#include	"LLlex.h"

/*	This file contains the (non-portable) error-message and diagnostic
	functions.  Beware, they are called with a variable number of
	arguments!
*/

/* error classes */
#define	ERROR		1
#define	WARNING		2
#define	LEXERROR	3
#define	LEXWARNING	4
#define	CRASH		5
#define	FATAL		6

int err_occurred;

extern char *symbol2str();
extern char options[];

/*	There are three general error-message functions:
		lexerror()	lexical and pre-processor error messages
		error()		syntactic and semantic error messages
		expr_error()	errors in expressions
	The difference lies in the place where the file name and line
	number come from.
	Lexical errors report from the global variables LineNumber and
	FileName, expression errors get their information from the
	expression, whereas other errors use the information in the token.
*/

/*VARARGS1*/
error(fmt, args)
	char *fmt;
{
	_error(ERROR, NILEXPR, fmt, &args);
}

/*VARARGS2*/
expr_error(expr, fmt, args)
	struct expr *expr;
	char *fmt;
{
	if (expr->ex_flags & EX_ERROR)
		return;			/* to prevent proliferation */
	_error(ERROR, expr, fmt, &args);
	expr->ex_flags |= EX_ERROR;
}

/*VARARGS1*/
warning(fmt, args)
	char *fmt;
{
	_error(WARNING, NILEXPR, fmt, &args);
}

/*VARARGS2*/
expr_warning(expr, fmt, args)
	struct expr *expr;
	char *fmt;
{
	if (expr->ex_flags & EX_ERROR)
		return;			/* to prevent proliferation */
	_error(WARNING, expr, fmt, &args);
}

/*VARARGS1*/
lexerror(fmt, args)
	char *fmt;
{
	_error(LEXERROR, NILEXPR, fmt, &args);
}

#ifndef	NOPP
/*VARARGS1*/
lexwarning(fmt, args) char *fmt;	{
	_error(LEXWARNING, NILEXPR, fmt, &args);
}
#endif	NOPP

/*VARARGS1*/
crash(fmt, args)
	char *fmt;
	int args;
{
	_error(CRASH, NILEXPR, fmt, &args);
	C_close();
#ifdef	DEBUG
	sys_stop(S_ABORT);
#else	DEBUG
	sys_stop(S_EXIT);
#endif	DEBUG
}

/*VARARGS1*/
fatal(fmt, args)
	char *fmt;
	int args;
{
#ifdef	USE_TMP
	extern char *tmpfile;	/* main.c	*/

	if (tmpfile)
		sys_remove(tmpfile);	/* may not successful!	*/
#endif	USE_TMP

	_error(FATAL, NILEXPR, fmt, &args);
	sys_stop(S_EXIT);
}

_error(class, expr, fmt, argv)
	int class;
	struct expr *expr;
	char *fmt;
	int argv[];
{
	/*	_error attempts to limit the number of error messages
		for a given line to MAXERR_LINE.
	*/
	static char *last_fn = 0;
	static unsigned int last_ln = 0;
	static int e_seen = 0;
	char *fn = 0;
	unsigned int ln = 0;
	char *remark = 0;
	
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
		if (C_busy())
			C_ms_err();
		err_occurred = 1;
		break;
	
	case WARNING:
	case LEXWARNING:
		if (options['w'])
			return;
		break;
	}

	/* the remark */
	switch (class)	{	
	case WARNING:
	case LEXWARNING:
		remark = "(warning)";
		break;
	case CRASH:
		remark = "CRASH\007";
		break;
	case FATAL:
		remark = "fatal error --";
		break;
	}
	
	/* the place */
	switch (class)	{	
	case WARNING:
	case ERROR:
		fn = expr ? expr->ex_file : dot.tk_file;
		ln = expr ? expr->ex_line : dot.tk_line;
		break;
	case LEXWARNING:
	case LEXERROR:
	case CRASH:
	case FATAL:
		fn = FileName;
		ln = LineNumber;
		break;
	}
	
	if (ln == last_ln && fn && last_fn && strcmp(fn, last_fn) == 0)	{
		/* we've seen this place before */
		e_seen++;
		if (e_seen == MAXERR_LINE)
			fmt = "etc ...";
		else
		if (e_seen > MAXERR_LINE)
			/* and too often, I'd say ! */
			return;
	}
	else	{
		/* brand new place */
		last_fn = fn;
		last_ln = ln;
		e_seen = 0;
	}
	
	if (fn)
		fprint(ERROUT, "\"%s\", line %u: ", fn, ln);
	if (remark)
		fprint(ERROUT, "%s ", remark);
	doprnt(ERROUT, fmt, argv);		/* contents of error */
	fprint(ERROUT, "\n");
}
