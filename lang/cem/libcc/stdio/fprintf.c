/* $Header$ */
#include <stdio.h>
#include <varargs.h>

fprintf(va_alist)
va_dcl
{
	FILE *file;
	char *fmt;
	va_list ap;

	va_start(ap);
	file = va_arg(ap, FILE *);
	fmt = va_arg(ap, char *);
	_doprnt (fmt, ap, file);
	if ( io_testflag(file,IO_PERPRINTF) )
        	fflush(file);
	va_end(ap);
}
