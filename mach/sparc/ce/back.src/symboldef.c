#include "header.h"

symbol_definition( s)
char *s;
{
	if (cur_seg == SEGTXT)
		flush_cache();	/* EXTRA */
	fprint( codefile, SYMBOL_DEF_FMT, s);
}
