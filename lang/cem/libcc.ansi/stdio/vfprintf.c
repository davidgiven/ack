/*
 * vfprintf - formatted output without ellipsis
 */
/* $Header$ */

#include	<stdio.h>
#include	<stdarg.h>
#include	"loc_incl.h"

int
vfprintf(FILE *stream, const char *format, va_list arg)
{
	int retval;

	retval = _doprnt (format, arg, stream);
	if (io_testflag(stream, _IOLBF))
		fflush(stream);

	return retval;
}
