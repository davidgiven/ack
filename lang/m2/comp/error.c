/* E R R O R   A N D   D I A G N O S T I C   R O U T I N E S */

/*	This file contains the (non-portable) error-message and diagnostic
	giving functions.  Be aware that they are called with a variable
	number of arguments!
*/

static char *RcsId = "$Header$";

#include	<system.h>
#include	<em_arith.h>
#include	"input.h"
#include	"f_info.h"
#include	"LLlex.h"

#define MAXERR_LINE	5	/* Number of error messages on one line ... */
#define	ERROUT		STDERR

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

#define NILEXPR	((struct expr *) 0)

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

#ifdef DEBUG
/*VARARGS2*/
debug(level, fmt, args)
	char *fmt;
{
	if (level <= options['D']) _error(VDEBUG, NILEXPR, fmt, &args);
}
#endif DEBUG

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
	_error(ERROR, expr, fmt, &args);
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
	_error(WARNING, expr, fmt, &args);
}

/*VARARGS1*/
lexerror(fmt, args)
	char *fmt;
{
	_error(LEXERROR, NILEXPR, fmt, &args);
}

/*VARARGS1*/
lexwarning(fmt, args) 
	char *fmt;
{
	_error(LEXWARNING, NILEXPR, fmt, &args);
}

/*VARARGS1*/
fatal(fmt, args)
	char *fmt;
	int args;
{

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
	static unsigned int last_ln = 0;
	static int e_seen = 0;
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
		/*
		if (C_busy())
			C_ms_err();
		*/
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
		ln = /* expr ? expr->ex_line : */ dot.tk_lineno;
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
	if (class != VDEBUG) {
#endif
	if (ln == last_ln)	{
		/* we've seen this place before */
		e_seen++;
		if (e_seen == MAXERR_LINE) fmt = "etc ...";
		else
		if (e_seen > MAXERR_LINE)
			/* and too often, I'd say ! */
			return;
	}
	else	{
		/* brand new place */
		last_ln = ln;
		e_seen = 0;
	}
#ifdef DEBUG
	}
#endif DEBUG
	
	if (FileName) fprintf(ERROUT, "\"%s\", line %u: ", FileName, ln);

	if (remark) fprintf(ERROUT, "%s ", remark);

	doprnt(ERROUT, fmt, argv);		/* contents of error */
	fprintf(ERROUT, "\n");
}
