/* $Header$ */
#include <stdio.h>
#include <varargs.h>

int fscanf(va_alist)
va_dcl
{
	FILE *fp;
	char *format;
	va_list ap;
	int retval;

	va_start(ap);
	fp = va_arg(ap, FILE *);
	format = va_arg(ap, char *);
	retval = _doscanf (fp, format, ap);
	va_end(ap);
	return retval;
}

