#include "header.h"
#include <em.h>

reloc1( s, o, r)
char *s;
arith o;
int r;
{
	fprint( codefile, RELOC1_FMT, s, (long) o);
}
