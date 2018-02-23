#include "sys.h"
.define	_stime
.extern	_errno

_stime:
	mov	2(sp),r1
	mov	(r1)+,r0
	mov	(r1),r1
	sys	stime
	bcc	1f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	clr	r0
	rts	pc
