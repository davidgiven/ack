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

#define IEEEFLOAT
#include <con_float>

/* as long as we generate assembler ...

do_open(filename)
	char *filename;
{
	if (filename == 0 || ! sys_open(filename, OP_WRITE, &codefile))
		return FALSE;

	fprint( codefile, ".sect .text; .sect .rom; .sect .data; .sect .bss\n");	return TRUE;
}
*/
