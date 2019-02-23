/* E R R O R   A N D   D I A G N O S T I C   R O U T I N E S */

/*	This file contains the (non-portable) error-message and diagnostic
	giving functions.  Be aware that they are called with a variable
	number of arguments!
*/

#include    "parameters.h"
#include	"debug.h"

#if __STDC__
#include	<stdarg.h>
#else
#include	<varargs.h>
#endif
#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    "print.h"
#include	"system.h"

#include	"LLlex.h"
#include	"f_info.h"
#include	"input.h"
#include	"main.h"
#include	"node.h"

/* error classes */
#define	ERROR		1
#define	WARNING		2
#define	LEXERROR	3
#define	LEXWARNING	4
#define	CRASH		5
#define	FATAL		6
#ifdef DEBUG
#define VDEBUG		7
#endif

int err_occurred;

static void _error(int, struct node *, char *, register va_list);

/*	There are three general error-message functions:
		lexerror()	lexical and pre-processor error messages
		error()		syntactic and pre-processor messagese
		node_error()	errors in nodes
	The difference lies in the place where the file name and line
	number come from.
	Lexical errors report from the global variables LineNumber and
	FileName, node errors get their information from the
	node, whereas other errors use the information in the token.
*/

#if __STDC__
#ifdef DEBUG
/*VARARGS*/
void debug(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(VDEBUG, NULLNODE, fmt, ap);
	}
	va_end(ap);
}
#endif /* DEBUG */

/*VARARGS*/
void error(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(ERROR, NULLNODE, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void node_error(struct node *node, char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(ERROR, node, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void warning(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(WARNING, NULLNODE, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void node_warning(struct node *node, char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(WARNING, node, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void lexerror(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(LEXERROR, NULLNODE, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void lexwarning(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(LEXWARNING, NULLNODE, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void fatal(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(FATAL, NULLNODE, fmt, ap);
	}
	va_end(ap);
	exit(EXIT_FAILURE);
}

/*VARARGS*/
void crash(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		_error(CRASH, NULLNODE, fmt, ap);
	}
	va_end(ap);
#ifdef DEBUG
	abort();
#else
	exit(EXIT_FAILURE);
#endif
}
#else
#ifdef DEBUG
/*VARARGS*/
void debug(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		_error(VDEBUG, NULLNODE, fmt, ap);
	}
	va_end(ap);
}
#endif /* DEBUG */

/*VARARGS*/
void error(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		_error(ERROR, NULLNODE, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void node_error(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		struct node *node = va_arg(ap, struct node *);
		char *fmt = va_arg(ap, char *);
		_error(ERROR, node, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void warning(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		_error(WARNING, NULLNODE, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void node_warning(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		struct node *node = va_arg(ap, struct node *);
		_error(WARNING, node, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void lexerror(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		_error(LEXERROR, NULLNODE, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void lexwarning(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		_error(LEXWARNING, NULLNODE, fmt, ap);
	}
	va_end(ap);
}

/*VARARGS*/
void fatal(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		_error(FATAL, NULLNODE, fmt, ap);
	}
	va_end(ap);
	exit(EXIT_FAILURE);
}

/*VARARGS*/
void crash(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);
		_error(CRASH, NULLNODE, fmt, ap);
	}
	va_end(ap);
#ifdef DEBUG
	abort();
#else
	exit(EXIT_FAILURE);
#endif
}
#endif

static void _error(int class, struct node *node, char *fmt, register va_list ap)
{
	/*	_error attempts to limit the number of error messages
		for a given line to MAXERR_LINE.
	*/
	static unsigned int last_ln = 0;
	unsigned int ln = 0;
	static char * last_fn = 0;
	static int e_seen = 0, w_seen = 0;
	register char *remark = 0;

	/*	Since name and number are gathered from different places
		depending on the class, we first collect the relevant
		values and then decide what to print.
	*/
	/* preliminaries */
	switch( class )	{
		case ERROR:
		case LEXERROR:
		case CRASH:
		case FATAL:
			if( C_busy() ) C_ms_err();
			err_occurred = 1;
			break;
	}

	/* the remark */
	switch( class )	{
		case WARNING:
		case LEXWARNING:
			if (options['w']) return;
			remark = "(warning)";
			break;
		case CRASH:
			remark = "CRASH\007";
			break;
		case FATAL:
			remark = "fatal error --";
			break;
#ifdef DEBUG
		case VDEBUG:
			remark = "(debug)";
			break;
#endif /* DEBUG */
	}

	/* the place */
	switch( class )	{
		case ERROR:
		case WARNING:
			ln = node ? node->nd_lineno : dot.tk_lineno;
			break;
		case LEXWARNING:
		case LEXERROR:
		case CRASH:
		case FATAL:
#ifdef DEBUG
		case VDEBUG:
#endif /* DEBUG */
			ln = LineNumber;
			break;
	}

#ifdef DEBUG
	if( class != VDEBUG )	{
#endif
	if( FileName == last_fn && ln == last_ln )	{
		/* we've seen this place before */
		if( class != WARNING && class != LEXWARNING ) {
			e_seen++;
			if( e_seen == MAXERR_LINE ) fmt = "etc ...";
			else if( e_seen > MAXERR_LINE )
				/* and too often, I'd say ! */
				return;
		}
		else {
			w_seen++;
			if( w_seen == MAXERR_LINE ) fmt = "etc ...";
			else if( w_seen > MAXERR_LINE )
				return;
		}
	}
	else	{
		/* brand new place */
		last_ln = ln;
		last_fn = FileName;
		e_seen = w_seen = 0;
	}
#ifdef DEBUG
	}
#endif /* DEBUG */

	if( FileName ) fprint(ERROUT, "\"%s\", line %u: ", FileName, ln);

	if( remark ) fprint(ERROUT, "%s ", remark);

	doprnt(ERROUT, fmt, ap);		/* contents of error */
	fprint(ERROUT, "\n");
}
