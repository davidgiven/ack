#include <system.h>

extern int nerrors;

error( fmt, argv)
char *fmt;
int argv;
{
	fprint( STDERR, "!! ERROR :	");
	doprnt( STDERR, fmt, &argv);
	fprint( STDERR, "	!!\n");
	nerrors++;
}
