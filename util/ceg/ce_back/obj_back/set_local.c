#include <out.h>
#include "back.h"
#include "header.h"

set_local_visible(sym)
char *sym;
{
	int i;

	i = find_sym(sym, REFERENCE);
 	symbol_table[i].on_type &= (~S_EXT);
}


