#include "header.h"

#ifndef OWN_SYMB
B_symbstr(s, len, type, add, val, added)
char *s, *val;
arith added;
{
	if (! s) s = "";
	fprint(codefile, ".symb \"%s\", %s+%ld, 0x%x, %d\n", s, val, (long) added, type, add);
}

B_symbcst(s, len, type, add, val)
char *s;
arith val;
{
	if (! s) s = "";
	fprint(codefile, ".symb \"%s\", %ld, 0x%x, %d\n", s, val, type, add);
}

B_symd(s, len, type, add)
char *s;
{
	if (! s) s = "";
	fprint(codefile, ".symd \"%s\", 0x%x, %d\n", s, type, add);
}
#endif
