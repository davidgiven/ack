#include "sys.h"
.define	_access
.extern	_errno

_access:
	mov	2(sp),0f+2
	mov	4(sp),0f+4
	clr	r0
	sys	indir; .data2 0f
	bcc	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
.sect .data
0:
	sys	access
	.data2	0, 0
