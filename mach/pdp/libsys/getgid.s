#include "sys.h"
.globl	_getgid
.globl	_getegid

_getgid:
	sys	getgid
	rts	pc

_getegid:
	sys	getgid
	mov	r1,r0
	rts	pc
