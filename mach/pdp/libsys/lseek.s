#include "sys.h"
.globl	_lseek
.globl	_errno

_lseek:
	mov	2(sp),r0
	mov	4(sp),0f+2
	mov	6(sp),0f+4
	mov	10(sp),0f+6
	sys	indir; 0f
	bec	1f
	mov	$-1,r1
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
.data
0:
	sys	lseek; ..; ..; ..
