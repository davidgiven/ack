#include "sys.h"
.globl	_submit
.globl	_errno

_submit:
	mov	2(sp),r0
	sys	local; 0f
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
.data
0:
	sys	submit
