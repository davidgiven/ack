#include <out.h>
#include "data.h"
#include "header.h"

set_global_visible(sym)
char *sym;
{
	int i;

	i = find_sym(sym, REFERENCE);
	symbol_table[i].on_type |= S_EXT;
}


