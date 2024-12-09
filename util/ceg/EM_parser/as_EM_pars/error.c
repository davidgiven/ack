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
	fprintf( stderr, "!! ERROR :	");
	vfprintf( stderr, fmt, pvar);
	fprintf( stderr, "	!!\n");
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
	fprintf( stderr, "!! ERROR :	");
	vfprintf( stderr, fmt, pvar);
	fprintf( stderr, "	!!\n");
	va_end(pvar);
	nerrors++;
}
#endif
