#include "sys.h"
.define	_ptrace
.extern	_errno

_ptrace:
	mov	2(sp),0f+6
	mov	4(sp),0f+2
	mov	6(sp),0f+4
	mov	010(sp),r0
	clr	_errno
	sys	indir; .data2 0f
	bcc	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc

.sect .data
0:
	sys	ptrace
	.data2	0, 0, 0
