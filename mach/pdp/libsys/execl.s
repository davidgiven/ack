#include "sys.h"
.globl	_execl
.globl	_environ
.globl	_errno

_execl:
	mov	2(sp),0f+2
	mov	sp,r0
	add	$4,r0
	mov	r0,0f+4
	mov	_environ,0f+6
	sys	indir; 0f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
.data
0:
	sys	exece; ..; ..; ..
