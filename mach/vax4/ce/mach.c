#include <system.h>
#include <em.h>
#include <out.h>
#include "mach.h"
#include "back.h"

arg_error( s, arg)
char *s;
int arg;
{
	fprint( STDERR, "arg_error %s %d\n", s, arg);
}
