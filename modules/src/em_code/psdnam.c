#include "em_private.h"

/* $Header$ */

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
