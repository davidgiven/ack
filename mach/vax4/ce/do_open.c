#include <system.h>

File *out_file;

open_back( filename)
char *filename;
{
	if ( filename == (char *) '\0')
		return( 0);
	else
		return( sys_open( filename, OP_WRITE, &out_file));
}

