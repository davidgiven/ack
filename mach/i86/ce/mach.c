#define CODE_EXPANDER
#include <system.h>
#include "back.h"
#include "mach.h"

#ifdef DEBUG
arg_error( s, arg)
char *s;
int arg;
{
	fprint( STDERR, "arg_error %s %d\n", s, arg);
}
#endif

int push_waiting = FALSE;

int fit_byte( val)
int val;
{
	return( val >= -128 && val <= 127);
}

#include <con_float>
