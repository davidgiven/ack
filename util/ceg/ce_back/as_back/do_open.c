#include "header.h"

do_open( filename)
char *filename;
{
	if ( filename == (char *) '\0')
		return( FALSE);
	else
		return( sys_open( filename, OP_WRITE, &codefile));
}
