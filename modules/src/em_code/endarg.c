#include "em_private.h"

/* $Id$ */

void CC_endnarg()
{
	/*	END pseudo of procedure with unknown # of locals
	*/
	PS(ps_end);
	CCEND();
	NL();
}
