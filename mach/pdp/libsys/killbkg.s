#include "sys.h"
.globl	_killbkg
.globl	_errno

_killbkg:
	mov	2(sp),0f+2
	mov	4(sp),0f+4
	sys	local; 0f
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	clr	r0
	rts	pc
.data
0:
	sys	killbkg; ..; ..
