#include "sys.h"
.globl	_write
.globl	_errno

_write:
	mov	2(sp),r0
	mov	4(sp),0f+2
	mov	6(sp),0f+4
	sys	indir; 0f
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
.data
0:
	sys	write; ..; ..
