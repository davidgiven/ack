#include <out.h>
#include <arch.h>
#include <ranlib.h>
#include "data.h"
#include "header.h"

do_open( filename)
char *filename;
{
	if ( filename == (char *) '\0')
		return( 0);
	else
		return( wr_open( filename));
}
