#include <out.h>
#include "back.h"

common(n)
arith n;
{
	extern int Label, Label_waiting;
	register struct outname *nm = &symbol_table[Label];

	if (Label_waiting) {
		/*	If possible, generate a common. Local commons must
			be delt with later.
		*/
		nm->on_type |= S_COM | (S_MIN+SEGBSS);
		if (n > nm->on_valu) {
			nm->on_valu = n;
		}
		Label_waiting = 0;
		return;
	}
	switchseg(SEGBSS);
	bss(n);
}
