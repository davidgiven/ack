#include "sys.h"
.globl _getppid

_getppid:
	sys	getpid
	mov	r1,r0
	rts	pc
