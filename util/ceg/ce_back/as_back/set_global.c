#include "header.h"

set_global_visible( s)
char *s;
{
	fprint( codefile, GLOBAL_FMT, s);
}
