#define CODE_EXPANDER
#include <em.h>
#include "back.h"

void
C_exa_dnam( s)
char *s;
{
	set_global_visible( extnd_dnam( s));
}
