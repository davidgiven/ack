#define CODE_EXPANDER
#include <em.h>
#include "back.h"

void
C_exp( s)
char *s;
{
	set_global_visible( extnd_name( s));
}
