#include "sys.h"
.globl	_nice
.globl	_errno

_nice:
	mov	2(sp),r0
	sys	nice
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	clr	r0
	rts	pc
