#include "sys.h"
.globl	_fetchi
.globl	_errno

_fetchi:
	mov	2(sp),r0
	sys	local; 0f
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
.data
0:
	sys	fetchi
