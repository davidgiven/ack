#define CODE_EXPANDER
#include <em.h>
#include "back.h"

void
C_ina_dlb( l)
label l;
{
	set_local_visible( extnd_dlb( l));
}
