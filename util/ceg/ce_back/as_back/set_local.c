#include "header.h"

set_local_visible( s)
char *s;
{
	fprint( codefile, LOCAL_FMT, s);
}
