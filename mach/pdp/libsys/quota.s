#include "sys.h"
.define	_quota
.extern	_errno

_quota:
	mov	2(sp),0f+2
	mov	4(sp),0f+4
	mov	6(sp),0f+6
	mov	010(sp),0f+010
	mov	012(sp),0f+012
	sys	local; .data2 0f
	bcc	1f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	clr	r0
	rts	pc
.sect .data
0:
	sys	quota
	.data2	0, 0, 0, 0, 0
