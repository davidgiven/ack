#include "header.h"

open_back( filename)
char *filename;
{
	if ( filename == (char *) 0)
		codefile= STDOUT;
	else
#ifndef sys_close
		return( sys_open( filename, OP_WRITE, &codefile));
#else
		return (codefile = fopen(filename, "w")) != NULL;
#endif
	return 1;
}
