#include "em_private.h"

/* $Header$ */

void
CC_pspnam(op, pnam)
	char *pnam;
{
	/*	Pseudo with procedure name
	*/
	PS(op);
	PNAM(pnam);
	NL();
}
