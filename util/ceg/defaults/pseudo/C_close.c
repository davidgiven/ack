#define CODE_EXPANDER
#include <em.h>
#include <assert.h>

extern int busy; 

C_close()
{
	assert( busy);

	end_back();
        close_back();
	busy = 0;
}
