/* $Header$ */
#include <stdio.h>
#include <varargs.h>

int scanf(va_alist)
va_dcl
{
	char *format;
	va_list ap;
	int retval;

	va_start(ap);
	format = va_arg(ap, char *);
	retval = _doscanf (stdin, format, ap);
	va_end(ap);
	return retval;
}


