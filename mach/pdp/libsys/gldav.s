#include "sys.h"
.globl	_gldav
.globl	_errno

_gldav:
	mov	2(sp),r0
	sys	local; 0f
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	clr	r0
	rts	pc
.data
0:
	sys	gldav
