#define CODE_EXPANDER
#include "mach.h"
#include <back.h>
#include <system.h>


#ifdef DEBUG
arg_error( s, arg)
char *s;
int arg;
{
	fprint( STDERR, "arg_error %s %d\n", s, arg);
}
#endif


/*
do_open( filename)
char *filename;
{
	if ( filename == (char *)0 || !sys_open( filename, OP_WRITE, &codefile))
		return( 0);

	fprint( codefile, ".sect .text; .sect .rom; .sect .data; .sect .bss\n");
	return( 1);
}
*/

#define IEEEFLOAT
#include <float_cst>
