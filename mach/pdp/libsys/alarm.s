#include "sys.h"
.globl	_alarm
.globl	_errno

_alarm:
	mov	2(sp),r0
	sys	alarm
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
