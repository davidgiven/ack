#include "sys.h"
.globl	_wait
.globl	_errno

_wait:
	sys	wait
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	tst	2(sp)
	beq	1f
	mov	r1,*2(sp)
1:
	rts	pc
