#include "header.h"

set_local_visible( s)
char *s;
{
	fprintf( codefile, LOCAL_FMT, s);
}
