#include <system.h>
#include <varargs.h>

extern int nerrors;

/*VARARGS*/
error(va_alist)
va_dcl
{
	char *fmt;
	va_list pvar;

	va_start(pvar);
	fmt = va_arg(pvar, char *);
	fprint( STDERR, "!! ERROR :	");
	doprnt( STDERR, fmt, pvar);
	fprint( STDERR, "	!!\n");
	va_end(pvar);
	nerrors++;
}
