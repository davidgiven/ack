#include "header.h"

bss( n)
arith n;
{
	fprint( codefile, BSS_FMT, (long) n);
}

common(n)
arith n;
{
	extern int saved;
	extern char labeltje[];

	if (saved) {
		fprint( codefile, COMM_FMT, labeltje, (long) n);
		saved = 0;
		return;
	}
	switchseg(SEGBSS);
	bss(n);
}
