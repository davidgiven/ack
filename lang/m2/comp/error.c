/*	E R R O R   A N D  D I A G N O S T I C   R O U T I N E S	*/

/*	This file contains the (non-portable) error-message and diagnostic
	giving functions.  Be aware that they are called with a variable
	number of arguments!
*/

#include	<stdio.h>
#include	"input.h"
#include	"f_info.h"
#include	"LLlex.h"

static char *RcsId = "$Header$";

#define	ERROUT	stderr

#define	ERROR		1
#define	WARNING		2
#define	LEXERROR	3
#define	LEXWARNING	4
#define	CRASH		5
#define	FATAL		6
#define	NONFATAL	7
#ifdef	DEBUG
#define	VDEBUG		8
#endif	DEBUG

int err_occurred;
/*
	extern int ofd;		/* compact.c	* /
	#define	compiling (ofd >= 0)
*/

extern char options[];

/*	There are two general error message giving functions:
	error() : syntactic and semantic error messages
	lexerror() : lexical and pre-processor error messages
	The difference lies in the fact that the first function deals with
	tokens already read in by the lexical analyzer so the name of the
	file it comes from and the linenumber must be retrieved from the
	token instead of looking at the global variables LineNumber and
	FileName.
*/

/*VARARGS1*/
error(fmt, args)
	char *fmt;
{
	/*
		if (compiling)
			C_ms_err();
	*/
	++err_occurred;
	_error(ERROR, fmt, &args);
}

#ifdef DEBUG
debug(fmt, args)
	char *fmt;
{
	if (options['D'])
		_error(VDEBUG, fmt, &args);
}
#endif DEBUG

/*VARARGS1*/
lexerror(fmt, args)
	char *fmt;
{
	/*
		if (compiling)
			C_ms_err();
	*/
	++err_occurred;
	_error(LEXERROR, fmt, &args);
}

/*VARARGS1*/
lexwarning(fmt, args) char *fmt;	{
	if (options['w']) return;
	_error(LEXWARNING, fmt, &args);
}

/*VARARGS1*/
crash(fmt, args)
	char *fmt;
	int args;
{
	/*
		if (compiling)
			C_ms_err();
	*/
	_error(CRASH, fmt, &args);
	fflush(ERROUT);
	fflush(stderr);
	fflush(stdout);
	/*
		cclose();
	*/
	abort();	/* produce core by "Illegal Instruction" */
			/* this should be changed into exit(1)	 */
}

/*VARARGS1*/
fatal(fmt, args)
	char *fmt;
	int args;
{
	/*
		if (compiling)
			C_ms_err();
	*/
	_error(FATAL, fmt, &args);
	exit(-1);
}

/*VARARGS1*/
nonfatal(fmt, args)
	char *fmt;
	int args;
{
	_error(NONFATAL, fmt, &args);
}

/*VARARGS1*/
warning(fmt, args)
	char *fmt;
{
	if (options['w']) return;
	_error(WARNING, fmt, &args);
}

_error(class, fmt, argv)
	int class;
	char *fmt;
	int argv[];
{

	switch (class)	{

	case ERROR:
	case LEXERROR:
		fprintf(ERROUT, "%s, line %ld: ", FileName, LineNumber);
		break;
	case WARNING:
	case LEXWARNING:
		fprintf(ERROUT, "%s, line %ld: (warning) ",
			FileName, LineNumber);
		break;
	case CRASH:
		fprintf(ERROUT, "CRASH\007 %s, line %ld: \n",
			FileName, LineNumber);
		break;
	case FATAL:
		fprintf(ERROUT, "%s, line %ld: fatal error -- ",
			FileName, LineNumber);
		break;
	case NONFATAL:
		fprintf(ERROUT, "warning: ");	/* no line number ??? */
		break;
#ifdef DEBUG
	case VDEBUG:
		fprintf(ERROUT, "-D ");
		break;
#endif DEBUG
	}
	_doprnt(fmt, argv, ERROUT);
	fprintf(ERROUT, "\n");
}
