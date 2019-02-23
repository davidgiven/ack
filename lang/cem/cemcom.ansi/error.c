/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	E R R O R   A N D  D I A G N O S T I C   R O U T I N E S	*/

#include	"parameters.h"
#include    "error.h"
#if __STDC__
#include	<stdarg.h>
#else
#include	<varargs.h>
#endif
#include	<system.h>
#ifndef	LINT
#include	<em.h>
#else
#include	"l_em.h"
#endif	/* LINT */
#include    <stdio.h>
#include	"tokenname.h"
#include	<flt_arith.h>
#include    "interface.h"
#include	"arith.h"
#include	"label.h"
#include	"expr.h"
#include	"def.h"
#include    "print.h"
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
#define DO_DEBUG	6

int err_occurred = 0;

extern char options[];
#ifdef	LINT
extern char loptions[];
#endif	/* LINT */

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
static void _error(int, char *, unsigned int, char*, va_list);

#if __STDC__
/*VARARGS*/
void error(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(ERROR, dot.tk_file, dot.tk_line, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void expr_error(struct expr *expr, char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		if (!(expr->ex_flags & EX_ERROR)) {
			/* to prevent proliferation */
			_error(ERROR, expr->ex_file, expr->ex_line, fmt, ap);
			expr->ex_flags |= EX_ERROR;
		}
	}
	va_end(ap);
}

/*VARARGS*/
void lexstrict(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(STRICT, FileName, LineNumber, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void strict(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(STRICT, dot.tk_file, dot.tk_line, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void expr_strict(struct expr *expr, char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		if (!(expr->ex_flags & EX_ERROR)) {
			/* to prevent proliferation */
			_error(STRICT, expr->ex_file, expr->ex_line, fmt, ap);
		}
	}
	va_end(ap);
}

#ifdef DEBUG
/*VARARGS*/
void debug(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(DO_DEBUG, dot.tk_file, dot.tk_line, fmt, ap);
	}
	va_end(ap);
}
#endif /* DEBUG */

/*VARARGS*/
void warning(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(WARNING, dot.tk_file, dot.tk_line, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void expr_warning(struct expr *expr, char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		if (!(expr->ex_flags & EX_ERROR)) {
			/* to prevent proliferation */
			_error(WARNING, expr->ex_file, expr->ex_line, fmt, ap);
		}
	}
	va_end(ap);
}

#ifdef	LINT

/*VARARGS*/
void def_warning(struct def *def, char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(WARNING, def->df_file, def->df_line, fmt, ap);
	}
	va_end(ap);
}


/*VARARGS*/
void hwarning(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		if (loptions['h'])
			_error(WARNING, dot.tk_file, dot.tk_line, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void awarning(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		if (loptions['a'])
			_error(WARNING, dot.tk_file, dot.tk_line, fmt, ap);
	}
	va_end(ap);
}

#endif	/* LINT */

/*VARARGS*/
void lexerror(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(ERROR, FileName, LineNumber, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void lexwarning(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(WARNING, FileName, LineNumber, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void crash(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(CRASH, FileName, LineNumber, fmt, ap);
	}
	va_end(ap);

	C_close();
#ifdef	DEBUG
	sys_stop(S_ABORT);
#else	/* DEBUG */
	sys_stop(S_EXIT);
#endif	/* DEBUG */
	/* NOTREACHED */
}

/*VARARGS*/
void fatal(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(FATAL, FileName, LineNumber, fmt, ap);
	}
	va_end(ap);

	if (C_busy()) C_close();
	sys_stop(S_EXIT);
	/*NOTREACHED*/
}
#else
/*VARARGS*/
void error(va_alist)				/* fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		_error(ERROR, dot.tk_file, dot.tk_line, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void expr_error(va_alist)			/* expr, fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		register struct expr *expr = va_arg(ap, struct expr *);
		char *fmt = va_arg(ap, char *);

		if (!(expr->ex_flags & EX_ERROR)) {
			/* to prevent proliferation */
			_error(ERROR, expr->ex_file, expr->ex_line, fmt, ap);
			expr->ex_flags |= EX_ERROR;
		}
	}
	va_end(ap);
}

/*VARARGS*/
void lexstrict(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		_error(STRICT, FileName, LineNumber, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void strict(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		_error(STRICT, dot.tk_file, dot.tk_line, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void expr_strict(va_alist)			/* expr, fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		struct expr *expr = va_arg(ap, struct expr *);
		char *fmt = va_arg(ap, char *);

		if (!(expr->ex_flags & EX_ERROR)) {
			/* to prevent proliferation */
			_error(STRICT, expr->ex_file, expr->ex_line, fmt, ap);
		}
	}
	va_end(ap);
}

#ifdef DEBUG
/*VARARGS*/
void debug(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		_error(DO_DEBUG, dot.tk_file, dot.tk_line, fmt, ap);
	}
	va_end(ap);
}
#endif /* DEBUG */

/*VARARGS*/
void warning(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		_error(WARNING, dot.tk_file, dot.tk_line, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void expr_warning(va_alist)			/* expr, fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		struct expr *expr = va_arg(ap, struct expr *);
		char *fmt = va_arg(ap, char *);

		if (!(expr->ex_flags & EX_ERROR)) {
			/* to prevent proliferation */
			_error(WARNING, expr->ex_file, expr->ex_line, fmt, ap);
		}
	}
	va_end(ap);
}

#ifdef	LINT

/*VARARGS*/
void def_warning(va_alist)			/* def, fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		register struct def *def = va_arg(ap, struct def *);
		char *fmt = va_arg(ap, char *);

		_error(WARNING, def->df_file, def->df_line, fmt, ap);
	}
	va_end(ap);
}


/*VARARGS*/
void hwarning(va_alist)			/* fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		if (loptions['h'])
			_error(WARNING, dot.tk_file, dot.tk_line, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void awarning(va_alist)			/* fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		if (loptions['a'])
			_error(WARNING, dot.tk_file, dot.tk_line, fmt, ap);
	}
	va_end(ap);
}

#endif	/* LINT */

/*VARARGS*/
void lexerror(va_alist)			/* fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		_error(ERROR, FileName, LineNumber, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void lexwarning(va_alist)			/* fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		_error(WARNING, FileName, LineNumber, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void crash(va_alist)				/* fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		_error(CRASH, FileName, LineNumber, fmt, ap);
	}
	va_end(ap);

	C_close();
#ifdef	DEBUG
	sys_stop(S_ABORT);
#else	/* DEBUG */
	sys_stop(S_EXIT);
#endif	/* DEBUG */
	/* NOTREACHED */
}

/*VARARGS*/
void fatal(va_alist)				/* fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		_error(FATAL, FileName, LineNumber, fmt, ap);
	}
	va_end(ap);

	if (C_busy()) C_close();
	sys_stop(S_EXIT);
	/*NOTREACHED*/
}
#endif

static void _error(int class, char *fn, unsigned int ln, char* fmt, va_list ap)
{
	char *remark;
	
	/* check visibility of message */
	switch (class)	{
	case WARNING:
	case ERROR:
	case STRICT:
		if (token_nmb < tk_nmb_at_last_syn_err + ERR_SHADOW)
			/* warning or error message overshadowed */
			return;
		break;
	}

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
#else	/* LINT */
		remark = 0;
#endif	/* LINT */
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
#ifdef DEBUG
	case DO_DEBUG:
		remark = "(debug)";
		break;
#endif /* DEBUG */
	default:
		/*NOTREACHED*/;
	}
	
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
#endif	/* LINT */
	
	if (fn)
		fprint(STDERR, "\"%s\", line %u: ", fn, ln);
	if (remark)
		fprint(STDERR, "%s ", remark);
	doprnt(STDERR, fmt, ap);		/* contents of error */
	fprint(STDERR, "\n");
}
