#include "header.h"
#include <em.h>

reloc4( s, o, r)
char *s;
arith o;
int r;
{
	fprint( codefile, RELOC4_FMT, s, (long) o);
}
