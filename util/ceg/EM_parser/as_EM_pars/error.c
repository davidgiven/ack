#include <system.h>
#if __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

extern int nerrors;

#if __STDC__
/*VARARGS*/
error(char *fmt, ...)
{
	va_list pvar;

	va_start(pvar, fmt);
	fprint( STDERR, "!! ERROR :	");
	doprnt( STDERR, fmt, pvar);
	fprint( STDERR, "	!!\n");
	va_end(pvar);
	nerrors++;
}
#else
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
#endif
