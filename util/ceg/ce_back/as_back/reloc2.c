#include "header.h"
#include <em.h>

reloc2( s, o, r)
char *s;
arith o;
int r;
{
	fprint( codefile, RELOC2_FMT, s, (long) o);
}
