#include "sys.h"
.define	__dup
.define	__dup2
.extern	_errno

__dup2:
	mov	2(sp),r0
	bis	$100,r0
	br	1f

__dup:
	mov	2(sp),r0
1:
	mov	4(sp),r1
	sys	dup
	bcc	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
