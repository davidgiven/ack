#include "sys.h"
.define	__pause

__pause:
	sys	pause
	rts	pc
