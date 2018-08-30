#include "sys.h"
.define	_lstat
.extern	_errno

_lstat:
	mov	2(sp),0f+2
	mov	4(sp),0f+4
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
	sys	lstat
	.data2	0, 0
