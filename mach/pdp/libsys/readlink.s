#include "sys.h"
.globl  _readlink
.globl	_errno

_readlink:
	mov	2(sp),r0
	mov	4(sp),0f+2
	mov	6(sp),0f+4
	sys	local; 0f
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
.data
0:
	sys	readlink; ..; ..
