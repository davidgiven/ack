/*
 * fprintf - write output on a stream
 */
/* $Header$ */

#include	<stdio.h>
#include	<stdarg.h>
#include	"loc_incl.h"

int
fprintf(FILE *stream, const char *format, ...)
{
	va_list ap;
	int retval;
	
	va_start(ap, format);

	retval = _doprnt (format, ap, stream);
	if ( io_testflag(stream,_IOLBF) )
		fflush(stream);

	va_end(ap);

	return retval;
}
