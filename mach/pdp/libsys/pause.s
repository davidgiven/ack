#include "sys.h"
.globl	_pause

_pause:
	sys	pause
	rts	pc
