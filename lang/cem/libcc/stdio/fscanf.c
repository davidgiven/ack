/* $Id$ */
#include <stdio.h>
#include <varargs.h>

int fscanf(va_alist)
	va_dcl
{
	va_list ap;
	int retval;

	va_start(ap);
	{
		FILE *fp = va_arg(ap, FILE *);
		char *format = va_arg(ap, char *);

		retval = _doscanf (fp, format, ap);
	}
	va_end(ap);

	return retval;
}

