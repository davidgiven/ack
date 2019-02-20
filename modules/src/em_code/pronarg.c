#include "em_private.h"

/* $Id$ */

void CC_pronarg(char* pnam)
{
	/*	PRO pseudo with procedure name pnam and unknown # of locals
	*/
	PS(ps_pro);
	PNAM(pnam);
	COMMA();
	CCEND();
	NL();
}
