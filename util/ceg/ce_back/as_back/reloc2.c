#include "header.h"

reloc2( s, o, r)
char *s;
arith o;
int r;
{
	fprintf( codefile, RELOC2_FMT, s, (long) o);
}
