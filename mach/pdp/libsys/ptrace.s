#include "sys.h"
.globl	_ptrace
.globl	_errno

_ptrace:
	mov	2(sp),0f+6
	mov	4(sp),0f+2
	mov	6(sp),0f+4
	mov	10(sp),r0
	clr	_errno
	sys	indir; 9f
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc

.data
0:
	sys	ptrace; ..; ..; ..
