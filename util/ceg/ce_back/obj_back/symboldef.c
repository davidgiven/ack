#include <out.h>
#include <system.h>
#include "data.h"
#include "back.h"
#include "header.h"

symbol_definition(sym)
char *sym;
{
	int i;

	i = find_sym( sym, SYMBOL_DEFINITION);
	(symbol_table + i)->on_valu = cur_value();
	(symbol_table + i)->on_type |= (S_MIN + conv_seg( cur_seg));
}

