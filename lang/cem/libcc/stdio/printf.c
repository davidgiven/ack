/* $Id$ */
#include <stdio.h>
#include <varargs.h>

printf(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);

		_doprnt (fmt, ap, stdout);
		if ( io_testflag(stdout,IO_PERPRINTF) )
        		fflush(stdout);
	}
	va_end(ap);
}
