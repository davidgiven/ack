#include "sys.h"
.define _getppid

_getppid:
	sys	getpid
	mov	r1,r0
	rts	pc
