#include "sys.h"
.globl	_getuid
.globl	_geteuid

_getuid:
	sys	getuid
	rts	pc

_geteuid:
	sys	getuid
	mov	r1,r0
	rts	pc
