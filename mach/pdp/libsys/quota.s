#include "sys.h"
.globl	_quota
.globl	_errno

_quota:
	mov	2(sp),0f+2
	mov	4(sp),0f+4
	mov	6(sp),0f+6
	mov	10(sp),0f+10
	mov	12(sp),0f+12
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
	sys	quota; ..; ..; ..; ..; ..
