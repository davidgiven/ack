#include "sys.h"
.globl	_getpid

_getpid:
	sys	getpid
	rts	pc
