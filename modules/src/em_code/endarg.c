#include "em_private.h"

CC_endnarg()
{
	/*	END pseudo of procedure with unknown # of locals
	*/
	PS(ps_end);
	CCEND();
	NL();
}
