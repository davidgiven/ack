#include "sys.h"
.globl	_pipe
.globl	_errno

_pipe:
	sys	pipe
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	mov	r5,-(sp)
	mov	4(sp),r5
	mov	r0,(r5)+
	mov	r1,(r5)
	mov	(sp)+,r5
	clr	r0
	rts	pc
