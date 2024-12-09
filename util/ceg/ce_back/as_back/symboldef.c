#include "header.h"

symbol_definition( s)
char *s;
{
	fprintf( codefile, SYMBOL_DEF_FMT, s);
}
