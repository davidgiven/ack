#include <system.h>
#include <back.h>

File *B_out_file;

open_back( filename)
char *filename;
{
	if ( filename == (char *) '\0')
		return( 0);
	else
		return( sys_open( filename, OP_WRITE, &B_out_file));
}

