/* $Id$ */
#include <stdio.h>
#include <varargs.h>

int scanf(va_alist)
	va_dcl
{
	va_list ap;
	int retval;

	va_start(ap);
	{
		char *format = va_arg(ap, char *);

		retval = _doscanf (stdin, format, ap);
	}
	va_end(ap);

	return retval;
}


