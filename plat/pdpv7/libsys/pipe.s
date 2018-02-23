#include "sys.h"
.define	_pipe
.extern	_errno

_pipe:
	sys	pipe
	bcc	1f
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
