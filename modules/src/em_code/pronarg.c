#include "em_private.h"

/* $Header$ */

void
CC_pronarg(pnam)
	char *pnam;
{
	/*	PRO pseudo with procedure name pnam and unknown # of locals
	*/
	PS(ps_pro);
	PNAM(pnam);
	COMMA();
	CCEND();
	NL();
}
