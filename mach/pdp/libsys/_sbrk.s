#include "sys.h"
.define	__sbrk
.define	__brk
.extern	_end
.extern	_errno

__sbrk:
	mov	2(sp),0f+2
	beq	1f
	add	Ixxx,0f+2
	bcs	2f
	sys	indir; .data2 0f
	bcc	1f
2:
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	mov	Ixxx,r0
	add	2(sp),Ixxx
	rts	pc

__brk:
	mov	2(sp),0f+2
	sys	indir; .data2 0f
	bcc	1f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	mov	2(sp),Ixxx
	clr	r0
	rts	pc

.sect .data
0:
	sys	break
	.data2	0
Ixxx:	.data2 _end
