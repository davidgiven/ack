#define CODE_EXPANDER
#include <em.h>
#include "back.h"

void
C_exa_dlb( l)
label l;
{
	set_global_visible( extnd_dlb( l));
}
