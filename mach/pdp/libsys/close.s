#include "sys.h"
.globl	_close
.globl	_errno

_close:
	mov	2(sp),r0
	sys	close
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	clr	r0
	rts	pc
