#include "header.h"
#include "back.h"
#include <em.h>
#include <out.h>

B_symbstr(s, len, type, add, val, added)
char *s, *val;
arith added;
{
	int i = find_sym(val, STORE_STRING);
	register struct outname *p;

	if (! s) s = "";
	p = &symbol_table[find_sym(s, FORCE_DEF)];
	p->on_type = S_CRS | (type << 8);
	p->on_desc = add;
	p->on_valu = i;
}

B_symbcst(s, len, type, add, val)
char *s;
arith val;
{
	register struct outname *p;

	if (! s) s = "";
	p = &symbol_table[find_sym(s, FORCE_DEF)];
	p->on_type = S_ABS | (type << 8);
	p->on_desc = add;
	p->on_valu = val;
}

B_symd(s, len, type, add)
char *s;
{
	register struct outname *p;

	if (! s) s = "";
	p = &symbol_table[find_sym(s, FORCE_DEF)];
	p->on_type = S_MIN | (type << 8);
	p->on_desc = add;
	p->on_valu = text-text_area;
}
