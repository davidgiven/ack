/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	E R R O R   A N D  D I A G N O S T I C   R O U T I N E S	*/

#include	<system.h>

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
	fprint(ERROUT, "\"%s\", line %d: %s", FileName, LineNumber, s);
}

/*VARARGS1*/
error(fmt, args)
	char *fmt;
{
	err_hdr("");
	doprnt(ERROUT, fmt, &args);
	fprint(ERROUT, "\n");
}

/*VARARGS1*/
warning(fmt, args)
	char *fmt;
{
	err_hdr("warning ");
	doprnt(ERROUT, fmt, &args);
	fprint(ERROUT, "\n");
}

/*VARARGS1*/
crash(fmt, args)
	char *fmt;
	int args;
{
	err_hdr("crash ");
	doprnt(ERROUT, fmt, &args);
	fprint(ERROUT, "\n");
	sys_stop(S_ABORT);
}

/*VARARGS1*/
fatal(fmt, args)
	char *fmt;
	int args;
{
	err_hdr("fatal ");
	doprnt(ERROUT, fmt, &args);
	fprint(ERROUT, "\n");
	sys_stop(S_EXIT);
}
