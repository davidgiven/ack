#include "sys.h"
.globl	_sbrk
.globl	_brk
.globl	_end
.globl	_errno

_sbrk:
	mov	2(sp),0f+2
	beq	1f
	add	xxx,0f+2
	bcs	2f
	sys	indir; 0f
	bec	1f
2:
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	mov	xxx,r0
	add	2(sp),xxx
	rts	pc

_brk:
	mov	2(sp),0f+2
	sys	indir; 0f
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	mov	2(sp),xxx
	clr	r0
	rts	pc

.data
0:
	sys	break; ..
xxx:	_end
