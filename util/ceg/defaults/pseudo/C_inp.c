#define CODE_EXPANDER
#include <em.h>
#include "back.h"

void
C_inp( s)
char *s;
{
	set_local_visible( extnd_name( s));
}
