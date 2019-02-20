#include "em_private.h"

/* $Id$ */

void CC_psdnam(int op, char* dnam)
{
	/*	Pseudo with data label
	*/
	PS(op);
	DNAM(dnam);
	NL();
}
