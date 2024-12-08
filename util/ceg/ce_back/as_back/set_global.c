#include "header.h"

set_global_visible( s)
char *s;
{
	fprintf( codefile, GLOBAL_FMT, s);
}
