#define CODE_EXPANDER
#include <em.h>
#include "back.h"

C_exa_dnam( s)
char *s;
{
	s = extnd_dnam( s);
	set_global_visible(s);
#ifdef __solaris__
	fprint(codefile, "\t.type\t%s,#object\n", s);
#endif
}
