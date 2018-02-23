#include "sys.h"
.define  _readlink
.extern	_errno

_readlink:
	mov	2(sp),r0
	mov	4(sp),0f+2
	mov	6(sp),0f+4
	sys	local; .data2 0f
	bcc	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
.sect .data
0:
	sys	readlink
	.data2	0, 0
