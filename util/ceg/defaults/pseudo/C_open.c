#include <assert.h>
#include <system.h>

extern int busy; 

int C_open( filename)
char *filename;
{
	assert( !busy);

	if ( !do_open( filename)) {
		fprint( STDERR, "Couldn't open %s\n", filename);
		return( 0)
	}
	else
	        return( 1);
}
