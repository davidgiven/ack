#include "sys.h"
.globl	_open
.globl	_errno

_open:
	mov	2(sp),0f+2
	mov	4(sp),0f+4
	sys	indir; 0f
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
.data
0:
	sys	open; ..; ..
