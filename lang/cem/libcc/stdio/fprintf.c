/* $Id$ */
#include <stdio.h>
#include <varargs.h>

fprintf(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		FILE *file = va_arg(ap, FILE *);
		char *fmt = va_arg(ap, char *);

		_doprnt (fmt, ap, file);
		if ( io_testflag(file,IO_PERPRINTF) )
        		fflush(file);
	}
	va_end(ap);
}
