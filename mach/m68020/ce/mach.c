#include "mach.h"
#include <back.h>
#include <stdio.h>


arg_error( s, arg)
char *s;
int arg;
{
	fprintf( stderr, "arg_error %s %d\n", s, arg);
}


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
