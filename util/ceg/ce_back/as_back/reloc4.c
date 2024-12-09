#include "header.h"

reloc4( s, o, r)
char *s;
arith o;
int r;
{
	fprintf( codefile, RELOC4_FMT, s, (long) o);
}
