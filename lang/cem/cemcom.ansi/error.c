/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*	E R R O R   A N D  D I A G N O S T I C   R O U T I N E S	*/

#include	<varargs.h>
#include	<system.h>
#include	<em.h>

#include	"lint.h"
#include	"nopp.h"
#include	"errout.h"
#include	"debug.h"

#include	"tokenname.h"
#include	"arith.h"
#include	"label.h"
#include	"expr.h"
#include	"def.h"
#include	"LLlex.h"

/*	This file contains the error-message and diagnostic
	functions.  Beware, they are called with a variable number of
	arguments!
*/

/* error classes */
#define	STRICT		1
#define	WARNING		2
#define	ERROR		3
#define	CRASH		4
#define	FATAL		5

int err_occurred = 0;

extern char options[];
#ifdef	LINT
extern char loptions[];
#endif	LINT

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

static _error();

/*VARARGS*/
error(va_alist)				/* fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		_error(ERROR, dot.tk_file, dot.tk_line, ap);
	}
	va_end(ap);
}

/*VARARGS*/
expr_error(va_alist)			/* expr, fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		register struct expr *expr = va_arg(ap, struct expr *);

		if (!(expr->ex_flags & EX_ERROR)) {
			/* to prevent proliferation */
			_error(ERROR, expr->ex_file, expr->ex_line, ap);
			expr->ex_flags |= EX_ERROR;
		}
	}
	va_end(ap);
}

/*VARARGS*/
strict(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		_error(STRICT, FileName, LineNumber, ap);
	}
	va_end(ap);
}

/*VARARGS*/
warning(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		_error(WARNING, NILEXPR, ap);
	}
	va_end(ap);
}

/*VARARGS*/
expr_warning(va_alist)			/* expr, fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		struct expr *expr = va_arg(ap, struct expr *);

		if (!(expr->ex_flags & EX_ERROR)) {
			/* to prevent proliferation */
			_error(WARNING, expr->ex_file, expr->ex_line, ap);
		}
	}
	va_end(ap);
}

#ifdef	LINT

/*VARARGS*/
def_warning(va_alist)			/* def, fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		register struct def *def = va_arg(ap, struct def *);

		_error(WARNING, def->df_file, def->df_line, ap);
	}
	va_end(ap);
}


/*VARARGS*/
hwarning(va_alist)			/* fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		if (loptions['h'])
			_error(WARNING, dot.tk_file, dot.tk_line, ap);
	}
	va_end(ap);
}

/*VARARGS*/
awarning(va_alist)			/* fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		if (loptions['a'])
			_error(WARNING, dot.tk_file, dot.tk_line, ap);
	}
	va_end(ap);
}

#endif	LINT

/*VARARGS*/
lexerror(va_alist)			/* fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		_error(ERROR, FileName, LineNumber, ap);
	}
	va_end(ap);
}

#ifndef	NOPP
/*VARARGS*/
lexwarning(va_alist)			/* fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		_error(WARNING, FileName, LineNumber, ap);
	}
	va_end(ap);
}
#endif	NOPP

/*VARARGS*/
crash(va_alist)				/* fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		_error(CRASH, FileName, LineNumber, ap);
	}
	va_end(ap);

	C_close();
#ifdef	DEBUG
	sys_stop(S_ABORT);
#else	DEBUG
	sys_stop(S_EXIT);
#endif	DEBUG
	/* NOTREACHED */
}

/*VARARGS*/
fatal(va_alist)				/* fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		_error(FATAL, FileName, LineNumber, ap);
	}
	va_end(ap);

	if (C_busy()) C_close();
	sys_stop(S_EXIT);
	/*NOTREACHED*/
}

static
_error(class, fn, ln, ap)
	int class;
	char *fn;
	unsigned int ln;
	va_list ap;
{
	/*	_error attempts to limit the number of error messages
		for a given line to MAXERR_LINE.
	*/
#ifndef	LINT
	static char *last_fn = 0;
	static unsigned int last_ln = 0;
	static int e_seen = 0;
#endif	LINT
	char *remark;
	char *fmt = va_arg(ap, char *);
	
	/*	Since name and number are gathered from different places
		depending on the class, we first collect the relevant
		values and then decide what to print.
	*/
	/* preliminaries */
	switch (class)	{
	case WARNING:
		if (options['w'])
			return;
		break;

	case STRICT:
		if (options['s'])
			return;
		break;

	case ERROR:
	case CRASH:
	case FATAL:
		if (C_busy())
			C_ms_err();
		err_occurred = 1;
		break;
	}

	/* the remark */
	switch (class)	{	
	case STRICT:
		remark = "(strict)";
		break;
	case WARNING:
#ifndef	LINT
		remark = "(warning)";
#else	LINT
		remark = 0;
#endif	LINT
		break;

	case ERROR:
		remark = 0;
		break;

	case CRASH:
		remark = "CRASH\007";
		break;

	case FATAL:
		remark = "fatal error --";
		break;
	default:
		/*NOTREACHED*/;
	}
	
#ifndef	LINT
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
#endif	LINT

#ifdef	LINT
	if (	/* there is a file name */
		fn
	&&	/* the file name is global */
		fn[0] == '/'
	&&	/* it is not a .c file */
		strcmp(&fn[strlen(fn)-2], ".c") != 0
	) {
		/* we skip this message */
		return;
	}
#endif	LINT
	
	if (fn)
		fprint(ERROUT, "\"%s\", line %u: ", fn, ln);
	if (remark)
		fprint(ERROUT, "%s ", remark);
	doprnt(ERROUT, fmt, ap);		/* contents of error */
	fprint(ERROUT, "\n");
}
