#include "sys.h"
.globl	_execve
.globl	_errno

_execve:
	mov	2(sp),0f+2
	mov	4(sp),0f+4
	mov	6(sp),0f+6
	sys	indir; 0f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
.data
0:
	sys	exece; ..; ..; ..
