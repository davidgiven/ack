#include <assert.h>

extern int busy; 

C_close()
{
	assert( busy);

	end_back();
        do_close();
	busy = 0;
}
