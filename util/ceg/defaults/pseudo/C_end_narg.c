#define CODE_EXPANDER
#include <em.h>
#include <assert.h>

extern int B_locals_created; 

C_end_narg()
{
	assert( B_locals_created);
}
