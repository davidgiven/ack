#include "sys.h"
.define	__creat
.extern	_errno

__creat:
	mov	2(sp),0f+2
	mov	4(sp),0f+4
	sys	indir; .data2 0f
	bcc	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
.sect .data
0:
	sys	creat
	.data2	0, 0
