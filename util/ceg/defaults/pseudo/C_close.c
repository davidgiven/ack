#define CODE_EXPANDER
#include <back.h>
#include <assert.h>

extern int B_busy; 

void
C_close()
{
	assert( B_busy);

	end_back();
        close_back();
	B_busy = 0;
}
