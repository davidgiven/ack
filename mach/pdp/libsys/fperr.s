#include "sys.h"
.globl	_fperr
.globl	_errno

_fperr:
	sys	local; 0f
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
	rts	pc
.data
0:
	sys	fperr
