#include "sys.h"
.define	_pause

_pause:
	sys	pause
	rts	pc
