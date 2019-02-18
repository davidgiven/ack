#include "em_private.h"

/* $Id$ */

void CC_pspnam(int op, char* pnam)
{
	/*	Pseudo with procedure name
	*/
	PS(op);
	PNAM(pnam);
	NL();
}
