/* E R R O R   A N D   D I A G N O S T I C   R O U T I N E S */

/*	This file contains the (non-portable) error-message and diagnostic
	giving functions.  Be aware that they are called with a variable
	number of arguments!
*/

#include	"debug.h"
#include	"errout.h"

#include	<em_arith.h>
#include	<em_code.h>
#include	<em_label.h>
#include	<system.h>

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

extern char *symbol2str();

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

#ifdef DEBUG
/*VARARGS1*/
debug(fmt, args)
	char *fmt;
{
	_error(VDEBUG, NULLNODE, fmt, &args);
}
#endif DEBUG

/*VARARGS1*/
error(fmt, args)
	char *fmt;
{
	_error(ERROR, NULLNODE, fmt, &args);
}

/*VARARGS2*/
node_error(node, fmt, args)
	struct node *node;
	char *fmt;
{
	_error(ERROR, node, fmt, &args);
}

/*VARARGS1*/
warning(fmt, args)
	char *fmt;
{
	if( !options['w'] ) _error(WARNING, NULLNODE, fmt, &args);
}

/*VARARGS2*/
node_warning(node, fmt, args)
	struct node *node;
	char *fmt;
{
	if( !options['w'] ) _error(WARNING, node, fmt, &args);
}

/*VARARGS1*/
lexerror(fmt, args)
	char *fmt;
{
	_error(LEXERROR, NULLNODE, fmt, &args);
}

/*VARARGS1*/
lexwarning(fmt, args) 
	char *fmt;
{
	if( !options['w'] ) _error(LEXWARNING, NULLNODE, fmt, &args);
}

/*VARARGS1*/
fatal(fmt, args)
	char *fmt;
{
	_error(FATAL, NULLNODE, fmt, &args);
	sys_stop(S_EXIT);
}

/*VARARGS1*/
crash(fmt, args)
	char *fmt;
{
	_error(CRASH, NULLNODE, fmt, &args);
#ifdef DEBUG
	sys_stop(S_ABORT);
#else
	sys_stop(S_EXIT);
#endif
}

_error(class, node, fmt, argv)
	int class;
	struct node *node;
	char *fmt;
	int argv[];
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
#endif DEBUG
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
#endif DEBUG
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
#endif DEBUG

	if( FileName ) fprint(ERROUT, "\"%s\", line %u: ", FileName, ln);

	if( remark ) fprint(ERROUT, "%s ", remark);

	doprnt(ERROUT, fmt, argv);		/* contents of error */
	fprint(ERROUT, "\n");
}
