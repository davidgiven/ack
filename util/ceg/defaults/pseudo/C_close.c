#define CODE_EXPANDER
#include <em.h>
#include <assert.h>

extern int B_busy; 

C_close()
{
	assert( B_busy);

	end_back();
        close_back();
	B_busy = 0;
}
