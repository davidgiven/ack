#include <out.h>
#include <arch.h>
#include <ranlib.h>
#include "back.h"
#include "header.h"

open_back( filename)
char *filename;
{
	if ( filename == (char *) '\0')
		return( 0);
	else
		return( wr_open( filename));
}
