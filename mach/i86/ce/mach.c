#include <system.h>
#include "mach.h"

arg_error( s, arg)
char *s;
int arg;
{
	fprint( STDERR, "arg_error %s %d\n", s, arg);
}

int push_waiting = FALSE;

int fit_byte( val)
int val;
{
	return( val >= -128 && val <= 127);
}
