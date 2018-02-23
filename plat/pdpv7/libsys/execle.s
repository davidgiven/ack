#include "sys.h"
.define	_execle
.extern	_errno

_execle:
	mov	2(sp),0f+2
	mov	sp,r0
	add	$4,r0
	mov	r0,0f+4
1:
	tst	(r0)+
	bne	1b
	mov	(r0),0f+6
	sys	indir; .data2 0f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
.sect .data
0:
	sys	exece
	.data2	0, 0, 0
