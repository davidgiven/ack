#include "em_private.h"

/* $Id$ */

void
CC_psdnam(op, dnam)
	char *dnam;
{
	/*	Pseudo with data label
	*/
	PS(op);
	DNAM(dnam);
	NL();
}
