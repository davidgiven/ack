#include "sys.h"
.define	__lseek
.extern	_errno

__lseek:
	mov	2(sp),r0
	mov	4(sp),0f+2
	mov	6(sp),0f+4
	mov	010(sp),0f+6
	sys	indir; .data2 0f
	bcc	1f
	mov	$-1,r1
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
.sect .data
0:
	sys	lseek
	.data2	0, 0, 0
