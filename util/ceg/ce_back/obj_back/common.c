#include <out.h>
#include <em.h>
#include "back.h"

common(n)
arith n;
{
	extern int _Label, _Label_waiting;
	register struct outname *nm = &symbol_table[_Label];

	if (_Label_waiting) {
		/*	If possible, generate a common. Local commons must
			be delt with later.
		*/
		nm->on_type |= S_COM | (S_MIN+SEGBSS);
		if (n > nm->on_valu) {
			nm->on_valu = n;
		}
		_Label_waiting = 0;
		return;
	}
	switchseg(SEGBSS);
	bss(n);
}
