#define CODE_EXPANDER
#include <em.h>

extern int B_busy; 

int C_busy()
{
	return( B_busy);
}
