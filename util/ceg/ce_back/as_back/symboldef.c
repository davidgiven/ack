#include "header.h"

symbol_definition( s)
char *s;
{
	fprint( codefile, SYMBOL_DEF_FMT, s);
}
