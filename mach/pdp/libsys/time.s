#include "sys.h"
.define	_time

_time:
	sys	time
	mov	r5,-(sp)
	mov	4(sp),r5
	beq	1f
	mov	r0,(r5)+
	mov	r1,(r5)+
1:
	mov	(sp)+,r5
	rts	pc
