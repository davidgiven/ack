#include "sys.h"
.globl	_setuid
.globl	_errno

_setuid:
	mov	2(sp),r0
	sys	setuid
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	clr	r0
	rts	pc
