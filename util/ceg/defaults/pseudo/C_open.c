#define CODE_EXPANDER
#include <em.h>
#include <assert.h>
#include <system.h>

extern int busy; 

int C_open( filename)
char *filename;
{
	assert( !busy);

	if ( !open_back( filename)) {
		fprint( STDERR, "Couldn't open %s\n", filename);
		return( 0);
	}
	else
	        return( 1);
}
