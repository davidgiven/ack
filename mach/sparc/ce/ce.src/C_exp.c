#define CODE_EXPANDER
#include <em.h>
#include "back.h"

C_exp( s)
char *s;
{
	s = extnd_name( s);
	set_global_visible(s);
#ifdef __solaris__
	fprint(codefile, "\t.type\t%s,#function\n", s);
#endif
}
