/*
 * vprintf - formatted output without ellipsis to the standard output stream
 */
/* $Header$ */

#include	<stdio.h>
#include	<stdarg.h>
#include	"loc_incl.h"

int
vprintf(const char *format, va_list arg)
{
	int retval;

	retval = _doprnt(format, arg, stdout);
	if (retval >= 0 && io_testflag(stdout, _IOLBF))
		if (fflush(stdout)) return EOF;

	return retval;
}
